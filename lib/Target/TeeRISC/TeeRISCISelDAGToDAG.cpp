//===-- TeeRISCISelDAGToDAG.cpp - A dag to dag inst selector for TeeRISC ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the TeeRISC target.
//
//===----------------------------------------------------------------------===//

#include "TeeRISCTargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

//===----------------------------------------------------------------------===//
// Instruction Selector Implementation
//===----------------------------------------------------------------------===//

//===--------------------------------------------------------------------===//
/// TeeRISCDAGToDAGISel - TeeRISC specific code to select TeeRISC machine
/// instructions for SelectionDAG operations.
///
namespace {
class TeeRISCDAGToDAGISel : public SelectionDAGISel {
  /// Subtarget - Keep a pointer to the TeeRISC Subtarget around so that we can
  /// make the right decision when generating code for different targets.
  const TeeRISCSubtarget &Subtarget;
  TeeRISCTargetMachine& TM;
public:
  explicit TeeRISCDAGToDAGISel(TeeRISCTargetMachine &tm)
    : SelectionDAGISel(tm),
      TM(tm),
      Subtarget(tm.getSubtarget<TeeRISCSubtarget>()) {
  }

  SDNode *Select(SDNode *N);

  // Complex Pattern Selectors.
  bool SelectADDRrr(SDValue N, SDValue &R1, SDValue &R2);

  /// SelectInlineAsmMemoryOperand - Implement addressing mode selection for
  /// inline asm expressions.
  virtual bool SelectInlineAsmMemoryOperand(const SDValue &Op,
                                            char ConstraintCode,
                                            std::vector<SDValue> &OutOps);

  virtual const char *getPassName() const {
    return "TeeRISC DAG->DAG Pattern Instruction Selection";
  }

  // Include the pieces autogenerated from the target description.
#include "TeeRISCGenDAGISel.inc"

private:
  SDNode* getGlobalBaseReg();
};
}  // end anonymous namespace

SDNode* TeeRISCDAGToDAGISel::getGlobalBaseReg() {
  unsigned GlobalBaseReg = TM.getInstrInfo()->getGlobalBaseReg(MF);
  return CurDAG->getRegister(GlobalBaseReg, TLI.getPointerTy()).getNode();
}

bool TeeRISCDAGToDAGISel::SelectADDRrr(SDValue Addr, SDValue &R1, SDValue &R2) {
  if (Addr.getOpcode() == ISD::FrameIndex) return false;
  if (Addr.getOpcode() == ISD::TargetExternalSymbol ||
      Addr.getOpcode() == ISD::TargetGlobalAddress)
    return false;  // direct calls.

  if (Addr.getOpcode() == ISD::ADD) {
    if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1)))
      if (isInt<16>(CN->getSExtValue()))
        return false;  // Let the reg+imm pattern catch this!
    R1 = Addr.getOperand(0);
    R2 = Addr.getOperand(1);
    return true;
  }

  R1 = Addr;
  R2 = CurDAG->getRegister(TeeRISC::ZERO, TLI.getPointerTy());
  return true;
}

SDNode *TeeRISCDAGToDAGISel::Select(SDNode *N) {
  DebugLoc dl = N->getDebugLoc();
  if (N->isMachineOpcode())
    return NULL;   // Already selected.

#if 0
  switch (N->getOpcode()) {
  default: break;
  case SPISD::GLOBAL_BASE_REG:
    return getGlobalBaseReg();

  case ISD::SDIV:
  case ISD::UDIV: {
    // sdivx / udivx handle 64-bit divides.
    if (N->getValueType(0) == MVT::i64)
      break;
    // FIXME: should use a custom expander to expose the SRA to the dag.
    SDValue DivLHS = N->getOperand(0);
    SDValue DivRHS = N->getOperand(1);

    // Set the Y register to the high-part.
    SDValue TopPart;
    if (N->getOpcode() == ISD::SDIV) {
      TopPart = SDValue(CurDAG->getMachineNode(SP::SRAri, dl, MVT::i32, DivLHS,
                                   CurDAG->getTargetConstant(31, MVT::i32)), 0);
    } else {
      TopPart = CurDAG->getRegister(SP::G0, MVT::i32);
    }
    TopPart = SDValue(CurDAG->getMachineNode(SP::WRYrr, dl, MVT::Glue, TopPart,
                                     CurDAG->getRegister(SP::G0, MVT::i32)), 0);

    // FIXME: Handle div by immediate.
    unsigned Opcode = N->getOpcode() == ISD::SDIV ? SP::SDIVrr : SP::UDIVrr;
    return CurDAG->SelectNodeTo(N, Opcode, MVT::i32, DivLHS, DivRHS,
                                TopPart);
  }
  case ISD::MULHU:
  case ISD::MULHS: {
    // FIXME: Handle mul by immediate.
    SDValue MulLHS = N->getOperand(0);
    SDValue MulRHS = N->getOperand(1);
    unsigned Opcode = N->getOpcode() == ISD::MULHU ? SP::UMULrr : SP::SMULrr;
    SDNode *Mul = CurDAG->getMachineNode(Opcode, dl, MVT::i32, MVT::Glue,
                                         MulLHS, MulRHS);
    // The high part is in the Y register.
    return CurDAG->SelectNodeTo(N, SP::RDY, MVT::i32, SDValue(Mul, 1));
  }
  }
#endif
  return SelectCode(N);
}


/// SelectInlineAsmMemoryOperand - Implement addressing mode selection for
/// inline asm expressions.
bool
TeeRISCDAGToDAGISel::SelectInlineAsmMemoryOperand(const SDValue &Op,
                                                  char ConstraintCode,
                                                  std::vector<SDValue> &OutOps) {
  SDValue Op0, Op1;
  switch (ConstraintCode) {
  default: return true;
  case 'm':   // memory
   if (!SelectADDRrr(Op, Op0, Op1))
       return true;
   break;
  }

  OutOps.push_back(Op0);
  OutOps.push_back(Op1);
  return false;
}

/// createTeeRISCISelDag - This pass converts a legalized DAG into a
/// TeeRISC-specific DAG, ready for instruction scheduling.
///
FunctionPass *llvm::createTeeRISCISelDag(TeeRISCTargetMachine &TM) {
  return new TeeRISCDAGToDAGISel(TM);
}
