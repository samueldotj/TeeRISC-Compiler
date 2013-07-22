//===-- TeeRISCBaseInfo.h - Top level definitions for TeeRISC -- --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone helper functions and enum definitions for
// the TeeRISC target useful for the compiler back-end and the MC libraries.
// As such, it deliberately does not include references to LLVM core
// code gen types, passes, etc..
//
//===----------------------------------------------------------------------===//

#ifndef TeeRISCBASEINFO_H
#define TeeRISCBASEINFO_H

#include "TeeRISCMCTargetDesc.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

static inline bool isTeeRISCRegister(unsigned Reg) {
  return Reg <= 31;
}

static inline bool isSpecialTeeRISCRegister(unsigned Reg) {
  switch (Reg) {
    default:
        return true;
    case TeeRISC::R0:
    case TeeRISC::R16...TeeRISC::R31:
      return false;
  }
}

/// getTeeRISCRegisterNumbering - Given the enum value for some register, e.g.
/// TeeRISC::R0, return the number that it corresponds to (e.g. 0).
static inline unsigned getTeeRISCRegisterNumbering(unsigned RegEnum) {
  switch (RegEnum) {
    case TeeRISC::ZERO   :
    case TeeRISC::R0     : return 0;
    case TeeRISC::R1     : return 1;
    case TeeRISC::R2     : return 2;
    case TeeRISC::R3     : return 3;
    case TeeRISC::R4     : return 4;
    case TeeRISC::R5     : return 5;
    case TeeRISC::R6     : return 6;
    case TeeRISC::R7     : return 7;
    case TeeRISC::R8     : return 8;
    case TeeRISC::R9     : return 9;
    case TeeRISC::R10    : return 10;
    case TeeRISC::R11    : return 11;
    case TeeRISC::R12    : return 12;
    case TeeRISC::R13    : return 13;
    case TeeRISC::FP     :
    case TeeRISC::R14    : return 14;
    case TeeRISC::SP     :
    case TeeRISC::R15    : return 15;
    case TeeRISC::R16    : return 16;
    case TeeRISC::R17    : return 17;
    case TeeRISC::R18    : return 18;
    case TeeRISC::R19    : return 19;
    case TeeRISC::R20    : return 20;
    case TeeRISC::R21    : return 21;
    case TeeRISC::R22    : return 22;
    case TeeRISC::R23    : return 23;
    case TeeRISC::R24    : return 24;
    case TeeRISC::R25    : return 25;
    case TeeRISC::R26    : return 26;
    case TeeRISC::R27    : return 27;
    case TeeRISC::R28    : return 28;
    case TeeRISC::R29    : return 29;
    case TeeRISC::R30    : return 30;
    case TeeRISC::R31    : return 31;
    default: llvm_unreachable("Unknown register number!");
  }
}

/// getRegisterFromNumbering - Given the enum value for some register, e.g.
/// TeeRISC::R0, return the number that it corresponds to (e.g. 0).
static inline unsigned getTeeRISCRegisterFromNumbering(unsigned Reg) {
  switch (Reg) {
    case 0  : return TeeRISC::ZERO;
    case 1  : return TeeRISC::R1;
    case 2  : return TeeRISC::R2;
    case 3  : return TeeRISC::R3;
    case 4  : return TeeRISC::R4;
    case 5  : return TeeRISC::R5;
    case 6  : return TeeRISC::R6;
    case 7  : return TeeRISC::R7;
    case 8  : return TeeRISC::R8;
    case 9  : return TeeRISC::R9;
    case 10 : return TeeRISC::R10;
    case 11 : return TeeRISC::R11;
    case 12 : return TeeRISC::R12;
    case 13 : return TeeRISC::R13;
    case 14 : return TeeRISC::FP;
    case 15 : return TeeRISC::SP;
    case 16 : return TeeRISC::IP;
    case 17 : return TeeRISC::LR;
    case 18 : return TeeRISC::R18;
    case 19 : return TeeRISC::R19;
    case 20 : return TeeRISC::R20;
    case 21 : return TeeRISC::R21;
    case 22 : return TeeRISC::R22;
    case 23 : return TeeRISC::R23;
    case 24 : return TeeRISC::R24;
    case 25 : return TeeRISC::R25;
    case 26 : return TeeRISC::R26;
    case 27 : return TeeRISC::R27;
    case 28 : return TeeRISC::R28;
    case 29 : return TeeRISC::R29;
    case 30 : return TeeRISC::R30;
    case 31 : return TeeRISC::R31;
    default: llvm_unreachable("Unknown register number!");
  }
}

} // end namespace llvm;

#endif
