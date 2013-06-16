//===-- TeeRISCSubtarget.cpp - TeeRISC Subtarget Information --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the TeeRISC specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "TeeRISCSubtarget.h"
#include "TeeRISC.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "TeeRISCGenSubtargetInfo.inc"

using namespace llvm;

void TeeRISCSubtarget::anchor() { }

TeeRISCSubtarget::TeeRISCSubtarget(const std::string &TT, const std::string &CPU,
                                   const std::string &FS,  bool is64Bit) :
  TeeRISCGenSubtargetInfo(TT, CPU, FS),
  IsV2(false) {
  std::string CPUName = CPU;
  if (CPUName.empty()) {
     CPUName = "v1";
  }

  // Parse features string.
  ParseSubtargetFeatures(CPUName, FS);
}
