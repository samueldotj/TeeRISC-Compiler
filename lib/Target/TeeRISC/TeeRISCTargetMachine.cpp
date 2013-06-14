//===-- TeeRISCTargetMachine.cpp - Define TargetMachine for TeeRISC -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

//#include "TeeRISCTargetMachine.h"
#include "TeeRISC.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

extern "C" void LLVMInitializeTeeRISCTarget() {
  // Register the target.
  //RegisterTargetMachine<TeeRISCTargetMachine> X(TheTeeRISCTarget);
}

