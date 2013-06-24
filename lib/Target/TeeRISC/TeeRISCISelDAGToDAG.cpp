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
  return CurDAG->getRegister(GlobalBaseReg, TLI->getPointerTy()).getNode();
}

SDNode *TeeRISCDAGToDAGISel::Select(SDNode *N) {
  DebugLoc dl = N->getDebugLoc();

  // If we have a custom node, we already have selected!
  if (N->isMachineOpcode())
    return NULL;   // Already selected.

  switch (N->getOpcode()) {
  default: break;
  case TeeRISC_ISD::GLOBAL_BASE_REG:
    return getGlobalBaseReg();
  }
  return SelectCode(N);
}

/// createTeeRISCISelDag - This pass converts a legalized DAG into a
/// TeeRISC-specific DAG, ready for instruction scheduling.
///
FunctionPass *llvm::createTeeRISCISelDag(TeeRISCTargetMachine &TM) {
  return new TeeRISCDAGToDAGISel(TM);
}
