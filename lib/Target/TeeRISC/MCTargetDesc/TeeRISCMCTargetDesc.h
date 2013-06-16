//===-- TeeRISCMCTargetDesc.h - TeeRISC Target Descriptions -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides TeeRISC specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef TeeRISCMCTARGETDESC_H
#define TeeRISCMCTARGETDESC_H

namespace llvm {
class Target;

extern Target TheTeeRISCTarget;

} // End llvm namespace

// Defines symbolic names for TeeRISC registers.  This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_ENUM
#include "TeeRISCGenRegisterInfo.inc"

// Defines symbolic names for the TeeRISC instructions.
//
#define GET_INSTRINFO_ENUM
#include "TeeRISCGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "TeeRISCGenSubtargetInfo.inc"

#endif
