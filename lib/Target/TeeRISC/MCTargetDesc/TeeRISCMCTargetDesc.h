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

#include "llvm/Support/DataTypes.h"

namespace llvm {
class MCAsmBackend;
class MCContext;
class MCCodeEmitter;
class MCInstrInfo;
class MCObjectWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class Target;
class StringRef;
class raw_ostream;

extern Target TheTeeRISCTarget;

MCCodeEmitter *createTeeRISCMCCodeEmitter(const MCInstrInfo &MCII,
                                          const MCRegisterInfo &MRI,
                                          const MCSubtargetInfo &STI,
                                          MCContext &Ctx);

MCAsmBackend *createTeeRISCAsmBackend(const Target &T, StringRef TT,
                                      StringRef CPU);

MCObjectWriter *createTeeRISCELFObjectWriter(raw_ostream &OS, uint8_t OSABI);

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
