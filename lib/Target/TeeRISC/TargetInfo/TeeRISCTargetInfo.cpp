//===-- TeeRISCTargetInfo.cpp - TeeRISC Target Implementation -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "TeeRISC.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheTeeRISCTarget;

extern "C" void LLVMInitializeTeeRISCTargetInfo() { 
  RegisterTarget<Triple::teerisc> X(TheTeeRISCTarget, "teerisc", "teerisc");
}

