//===-- TeeRISCISelLowering.h - TeeRISC DAG Lowering Interface --*- C++ -*-===//
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

#ifndef TeeRISCISELLOWERING_H
#define TeeRISCISELLOWERING_H

#include "TeeRISC.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/Target/TargetLowering.h"

namespace llvm {
  class TeeRISCSubtarget;
  namespace TeeRISC_ISD {
    enum NodeType {
      // Start the numbering from where ISD NodeType finishes.
      FIRST_NUMBER = ISD::BUILTIN_OP_END,
      CALL,             // A call instruction.
      RET,              // A return instruction
      GLOBAL_BASE_REG   // Global base reg for PIC
    };
  }

  //===--------------------------------------------------------------------===//
  // TargetLowering Implementation
  //===--------------------------------------------------------------------===//

  class TeeRISCTargetLowering : public TargetLowering  {
    const TeeRISCSubtarget *Subtarget;
  public:
    explicit TeeRISCTargetLowering(TeeRISCTargetMachine &TM);

    virtual const char *getTargetNodeName(unsigned Opcode) const;

  private:
	//- must be exist without function all
    virtual SDValue
      LowerFormalArguments(SDValue Chain,
                           CallingConv::ID CallConv, bool isVarArg,
                           const SmallVectorImpl<ISD::InputArg> &Ins,
                           SDLoc dl, SelectionDAG &DAG,
                           SmallVectorImpl<SDValue> &InVals) const;

    virtual SDValue
      LowerCall(TargetLowering::CallLoweringInfo &CLI,
                SmallVectorImpl<SDValue> &InVals) const;

    virtual SDValue
      LowerCallResult(SDValue Chain, SDValue InFlag, CallingConv::ID CallConv,
                      bool isVarArg, const SmallVectorImpl<ISD::InputArg> &Ins,
                      SDLoc dl, SelectionDAG &DAG,
                      SmallVectorImpl<SDValue> &InVals) const;

	//- must be exist without function all
    virtual SDValue
      LowerReturn(SDValue Chain,
                  CallingConv::ID CallConv, bool isVarArg,
                  const SmallVectorImpl<ISD::OutputArg> &Outs,
                  const SmallVectorImpl<SDValue> &OutVals,
                  SDLoc dl, SelectionDAG &DAG) const;
    virtual SDValue 
      LowerOperation(SDValue Op, SelectionDAG &DAG) const;
  };
}

#endif // TeeRISCISELLOWERING_H
