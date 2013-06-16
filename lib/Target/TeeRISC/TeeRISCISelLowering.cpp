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
#include "TeeRISCTargetMachine.h"
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

  //- Set .align 2
  // It will emit .align 2 later
  setMinFunctionAlignment(2);

  // must, computeRegisterProperties - Once all of the register classes are 
  //  added, this allows us to compute derived properties we expose.
  computeRegisterProperties();
}

#include "TeeRISCGenCallingConv.inc"

/// LowerFormalArguments - transform physical registers into virtual registers
/// and generate load operations for arguments places on the stack.
SDValue
TeeRISCTargetLowering::LowerFormalArguments(SDValue Chain,
                                            CallingConv::ID CallConv,
                                            bool isVarArg,
                                            const SmallVectorImpl<ISD::InputArg> &Ins,
                                            DebugLoc dl, SelectionDAG &DAG,
                                            SmallVectorImpl<SDValue> &InVals) const {
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
                                   DebugLoc dl, SelectionDAG &DAG) const {

    return DAG.getNode(TeeRISC::LD, dl, MVT::Other,
                       Chain, DAG.getRegister(TeeRISC::LR, MVT::i32));
}

