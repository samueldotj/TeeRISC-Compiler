//===-- TeeRISCISelLowering.cpp - TeeRISC DAG Lowering Implementation -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that TeeRISC uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "TeeRISC-lower"
#include "TeeRISCISelLowering.h"
#include "TeeRISCSubtarget.h"
#include "TeeRISCTargetMachine.h"
#include "MCTargetDesc/TeeRISCBaseInfo.h"
#include "TeeRISCMachineFunctionInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

TeeRISCTargetLowering::TeeRISCTargetLowering(TeeRISCTargetMachine &TM)
    : TargetLowering(TM, new TargetLoweringObjectFileELF()) {
  
  // Set up the register classes
  addRegisterClass(MVT::i32, &TeeRISC::IntRegsRegClass);

  // Variable Argument support
  setOperationAction(ISD::VASTART,            MVT::Other, Custom);
  setOperationAction(ISD::VAEND,              MVT::Other, Expand);
  setOperationAction(ISD::VAARG,              MVT::Other, Expand);
  setOperationAction(ISD::VACOPY,             MVT::Other, Expand);

  //- Set .align 2
  // It will emit .align 2 later
  setMinFunctionAlignment(2);

  // must, computeRegisterProperties - Once all of the register classes are 
  //  added, this allows us to compute derived properties we expose.
  computeRegisterProperties();
}

#include "TeeRISCGenCallingConv.inc"

const char *TeeRISCTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  default: return 0;
  case TeeRISC_ISD::RET:    return "TeeRISC_ISD::RET";
  }
}

//===----------------------------------------------------------------------===//
//                  Call Calling Convention Implementation
//===----------------------------------------------------------------------===//

/// LowerCall - functions arguments are copied from virtual regs to
/// (physical regs)/(stack frame), CALLSEQ_START and CALLSEQ_END are emitted.
/// TODO: isVarArg, isTailCall.
SDValue TeeRISCTargetLowering::
LowerCall(TargetLowering::CallLoweringInfo &CLI,
          SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG                     = CLI.DAG;
  SDLoc dl                              = CLI.DL;
  SmallVector<ISD::OutputArg, 32> &Outs = CLI.Outs;
  SmallVector<SDValue, 32> &OutVals     = CLI.OutVals;
  SmallVector<ISD::InputArg, 32> &Ins   = CLI.Ins;
  SDValue Chain                         = CLI.Chain;
  SDValue Callee                        = CLI.Callee;
  bool &isTailCall                      = CLI.IsTailCall;
  CallingConv::ID CallConv              = CLI.CallConv;
  bool isVarArg                         = CLI.IsVarArg;

  // TeeRISC does not yet support tail call optimization
  isTailCall = false;

  // The TeeRISC requires stack slots for arguments passed to var arg
  // functions even if they are passed in registers.
  bool needsRegArgSlots = isVarArg;

  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  const TargetFrameLowering &TFI = *MF.getTarget().getFrameLowering();

  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
                 getTargetMachine(), ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeCallOperands(Outs, CC_TeeRISC);

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = CCInfo.getNextStackOffset();

  // Variable argument function calls require a minimum of 24-bytes of stack
  if (isVarArg && NumBytes < 24) NumBytes = 24;

  Chain = DAG.getCALLSEQ_START(Chain, DAG.getIntPtrConstant(NumBytes, true),
                               dl);

  SmallVector<std::pair<unsigned, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;

  // Walk the register/memloc assignments, inserting copies/loads.
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    MVT RegVT = VA.getLocVT();
    SDValue Arg = OutVals[i];

    // Promote the value if needed.
    switch (VA.getLocInfo()) {
    default: llvm_unreachable("Unknown loc info!");
    case CCValAssign::Full: break;
    case CCValAssign::SExt:
      Arg = DAG.getNode(ISD::SIGN_EXTEND, dl, RegVT, Arg);
      break;
    case CCValAssign::ZExt:
      Arg = DAG.getNode(ISD::ZERO_EXTEND, dl, RegVT, Arg);
      break;
    case CCValAssign::AExt:
      Arg = DAG.getNode(ISD::ANY_EXTEND, dl, RegVT, Arg);
      break;
    }

    // Arguments that can be passed on register must be kept at
    // RegsToPass vector
    if (VA.isRegLoc()) {
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
    } else {
      // Register can't get to this point...
      assert(VA.isMemLoc());

      // Since we are alread passing values on the stack we don't
      // need to worry about creating additional slots for the
      // values passed via registers.
      needsRegArgSlots = false;

      // Create the frame index object for this incoming parameter
      unsigned ArgSize = VA.getValVT().getSizeInBits()/8;
      unsigned StackLoc = VA.getLocMemOffset() + 4;
      int FI = MFI->CreateFixedObject(ArgSize, StackLoc, true);

      SDValue PtrOff = DAG.getFrameIndex(FI,getPointerTy());

      // emit ISD::STORE whichs stores the
      // parameter value to a stack Location
      MemOpChains.push_back(DAG.getStore(Chain, dl, Arg, PtrOff,
                                         MachinePointerInfo(),
                                         false, false, 0));
    }
  }

  // If we need to reserve stack space for the arguments passed via registers
  // then create a fixed stack object at the beginning of the stack.
  if (needsRegArgSlots && TFI.hasReservedCallFrame(MF))
    MFI->CreateFixedObject(28, 0, true);

  // Transform all store nodes into one single node because all store
  // nodes are independent of each other.
  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other,
                        &MemOpChains[0], MemOpChains.size());

  // Build a sequence of copy-to-reg nodes chained together with token
  // chain and flag operands which copy the outgoing args into registers.
  // The InFlag in necessary since all emitted instructions must be
  // stuck together.
  SDValue InFlag;
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
    Chain = DAG.getCopyToReg(Chain, dl, RegsToPass[i].first,
                             RegsToPass[i].second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // If the callee is a GlobalAddress/ExternalSymbol node (quite common, every
  // direct call is) turn it into a TargetGlobalAddress/TargetExternalSymbol
  // node so that legalize doesn't hack it.
  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
    Callee = DAG.getTargetGlobalAddress(G->getGlobal(), dl,
                                getPointerTy(), 0, 0);
  else if (ExternalSymbolSDNode *S = dyn_cast<ExternalSymbolSDNode>(Callee))
    Callee = DAG.getTargetExternalSymbol(S->getSymbol(),
                                getPointerTy(), 0);

  // TeeRISC::CALL = #chain, #target_address, #opt_in_flags...
  //               = Chain, Callee, Reg#1, Reg#2, ...
  //
  // Returns a chain & a flag for retval copy to use.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
    Ops.push_back(DAG.getRegister(RegsToPass[i].first,
                                  RegsToPass[i].second.getValueType()));
  }

  if (InFlag.getNode())
    Ops.push_back(InFlag);

  Chain  = DAG.getNode(TeeRISC_ISD::CALL, dl, NodeTys, &Ops[0], Ops.size());
  InFlag = Chain.getValue(1);

  // Create the CALLSEQ_END node.
  Chain = DAG.getCALLSEQ_END(Chain, DAG.getIntPtrConstant(NumBytes, true),
                             DAG.getIntPtrConstant(0, true), InFlag, dl);
  if (!Ins.empty())
    InFlag = Chain.getValue(1);

  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return LowerCallResult(Chain, InFlag, CallConv, isVarArg,
                         Ins, dl, DAG, InVals);
}

/// LowerCallResult - Lower the result values of a call into the
/// appropriate copies out of appropriate physical registers.
SDValue TeeRISCTargetLowering::
LowerCallResult(SDValue Chain, SDValue InFlag, CallingConv::ID CallConv,
                bool isVarArg, const SmallVectorImpl<ISD::InputArg> &Ins,
                SDLoc dl, SelectionDAG &DAG,
                SmallVectorImpl<SDValue> &InVals) const {
  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
                 getTargetMachine(), RVLocs, *DAG.getContext());

  CCInfo.AnalyzeCallResult(Ins, RetCC_TeeRISC);

  // Copy all of the result registers out of their specified physreg.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    Chain = DAG.getCopyFromReg(Chain, dl, RVLocs[i].getLocReg(),
                               RVLocs[i].getValVT(), InFlag).getValue(1);
    InFlag = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
//             Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

/// LowerFormalArguments - transform physical registers into virtual registers
/// and generate load operations for arguments places on the stack.
SDValue
TeeRISCTargetLowering::LowerFormalArguments(SDValue Chain,
                                            CallingConv::ID CallConv, bool isVarArg,
                                            const SmallVectorImpl<ISD::InputArg> &Ins,
                                            SDLoc dl, SelectionDAG &DAG,
                                            SmallVectorImpl<SDValue> &InVals) const {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  TeeRISCMachineFunctionInfo *TFI = MF.getInfo<TeeRISCMachineFunctionInfo>();

  unsigned StackReg = MF.getTarget().getRegisterInfo()->getFrameRegister(MF);
  TFI->setVarArgsFrameIndex(0);

  // Used with vargs to acumulate store chains.
  std::vector<SDValue> OutChains;

  // Keep track of the last register used for arguments
  unsigned ArgRegEnd = 0;

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
                 getTargetMachine(), ArgLocs, *DAG.getContext());

  CCInfo.AnalyzeFormalArguments(Ins, CC_TeeRISC);
  SDValue StackPtr;

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];

    // Arguments stored on registers
    if (VA.isRegLoc()) {
      MVT RegVT = VA.getLocVT();
      ArgRegEnd = VA.getLocReg();
      const TargetRegisterClass *RC;

      if (RegVT == MVT::i32)
        RC = &TeeRISC::IntRegsRegClass;
      else
        llvm_unreachable("RegVT not supported by LowerFormalArguments");

      // Transform the arguments stored on
      // physical registers into virtual ones
      unsigned Reg = MF.addLiveIn(ArgRegEnd, RC);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, dl, Reg, RegVT);

      // If this is an 8 or 16-bit value, it has been passed promoted
      // to 32 bits.  Insert an assert[sz]ext to capture this, then
      // truncate to the right size. If if is a floating point value
      // then convert to the correct type.
      if (VA.getLocInfo() != CCValAssign::Full) {
        unsigned Opcode = 0;
        if (VA.getLocInfo() == CCValAssign::SExt)
          Opcode = ISD::AssertSext;
        else if (VA.getLocInfo() == CCValAssign::ZExt)
          Opcode = ISD::AssertZext;
        if (Opcode)
          ArgValue = DAG.getNode(Opcode, dl, RegVT, ArgValue,
                                 DAG.getValueType(VA.getValVT()));
        ArgValue = DAG.getNode(ISD::TRUNCATE, dl, VA.getValVT(), ArgValue);
      }

      InVals.push_back(ArgValue);
    } else { // VA.isRegLoc()
      // sanity check
      assert(VA.isMemLoc());

      // The last argument is not a register
      ArgRegEnd = 0;

      // The stack pointer offset is relative to the caller stack frame.
      // Since the real stack size is unknown here, a negative SPOffset
      // is used so there's a way to adjust these offsets when the stack
      // size get known (on EliminateFrameIndex). A dummy SPOffset is
      // used instead of a direct negative address (which is recorded to
      // be used on emitPrologue) to avoid mis-calc of the first stack
      // offset on PEI::calculateFrameObjectOffsets.
      // Arguments are always 32-bit.
      unsigned ArgSize = VA.getLocVT().getSizeInBits() / 8;
      unsigned StackLoc = VA.getLocMemOffset() + 4;
      int FI = MFI->CreateFixedObject(ArgSize, 0, true);
      TFI->recordLoadArgsFI(FI, -StackLoc);
      TFI->recordLiveIn(FI);

      // Create load nodes to retrieve arguments from the stack
      SDValue FIN = DAG.getFrameIndex(FI, getPointerTy());
      InVals.push_back(DAG.getLoad(VA.getValVT(), dl, Chain, FIN,
                                   MachinePointerInfo::getFixedStack(FI),
                                   false, false, false, 0));
    }
  }

  // To meet ABI, when VARARGS are passed on registers, the registers
  // must have their values written to the caller stack frame. If the last
  // argument was placed in the stack, there's no need to save any register.
  if ((isVarArg) && ArgRegEnd) {
    if (StackPtr.getNode() == 0)
      StackPtr = DAG.getRegister(StackReg, getPointerTy());

    // The last register argument that must be saved is TeeRISC::R5
    const TargetRegisterClass *RC = &TeeRISC::IntRegsRegClass;

    unsigned Begin = getTeeRISCRegisterNumbering(TeeRISC::R2);
    unsigned Start = getTeeRISCRegisterNumbering(ArgRegEnd+1);
    unsigned End   = getTeeRISCRegisterNumbering(TeeRISC::R5);
    unsigned StackLoc = Start - Begin + 1;

    for (; Start <= End; ++Start, ++StackLoc) {
      unsigned Reg = getTeeRISCRegisterFromNumbering(Start);
      unsigned LiveReg = MF.addLiveIn(Reg, RC);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, dl, LiveReg, MVT::i32);

      int FI = MFI->CreateFixedObject(4, 0, true);
      TFI->recordStoreVarArgsFI(FI, -(StackLoc * 4));
      SDValue PtrOff = DAG.getFrameIndex(FI, getPointerTy());
      OutChains.push_back(DAG.getStore(Chain, dl, ArgValue, PtrOff,
                                       MachinePointerInfo(),
                                       false, false, 0));

      // Record the frame index of the first variable argument
      // which is a value necessary to VASTART.
      if (!TFI->getVarArgsFrameIndex())
        TFI->setVarArgsFrameIndex(FI);
    }
  }

  // All stores are grouped in one node to allow the matching between
  // the size of Ins and InVals. This only happens when on varg functions
  if (!OutChains.empty()) {
    OutChains.push_back(Chain);
    Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other,
                        &OutChains[0], OutChains.size());
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
//               Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

SDValue
TeeRISCTargetLowering::LowerReturn(SDValue Chain,
                                   CallingConv::ID CallConv, bool isVarArg,
                                   const SmallVectorImpl<ISD::OutputArg> &Outs,
                                   const SmallVectorImpl<SDValue> &OutVals,
                                   SDLoc dl, SelectionDAG &DAG) const {
  // CCValAssign - represent the assignment of
  // the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;
  MachineFunction &MF = DAG.getMachineFunction();

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, isVarArg, MF, getTargetMachine(), RVLocs,
                 *DAG.getContext());

  // Analyze return values.
  CCInfo.AnalyzeReturn(Outs, RetCC_TeeRISC);

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  RetOps.push_back(DAG.getRegister(TeeRISC::R1, MVT::i32));

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    SDValue Val = OutVals[i];
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    if (RVLocs[i].getValVT() != RVLocs[i].getLocVT())
      Val = DAG.getNode(ISD::BITCAST, dl, RVLocs[i].getLocVT(), Val);

    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), Val, Flag);

    // Guarantee that all emitted copies are stuck together with flags.
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain;  // Update chain.

  // Add the flag if we have it.
  if (Flag.getNode())
    RetOps.push_back(Flag);

  return DAG.getNode(TeeRISC_ISD::RET, dl, MVT::Other, &RetOps[0], RetOps.size());
}

/// LowerOperation - Handle custom opcodes
SDValue
TeeRISCTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {

  assert(0 && "LowerOperation() is Not implemented");  
  return SDValue();
}

