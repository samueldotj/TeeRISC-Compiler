//===-- TeeRISCMCAsmInfo.h - TeeRISC asm properties ------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the TeeRISCMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef TeeRISCTARGETASMINFO_H
#define TeeRISCTARGETASMINFO_H

#include "llvm/MC/MCAsmInfo.h"

namespace llvm {
  class StringRef;

  class TeeRISCELFMCAsmInfo : public MCAsmInfo {
    virtual void anchor();
  public:
    explicit TeeRISCELFMCAsmInfo(StringRef TT);
  };

} // namespace llvm

#endif
