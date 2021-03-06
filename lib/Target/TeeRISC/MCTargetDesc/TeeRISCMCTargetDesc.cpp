//===-- TeeRISCMCTargetDesc.cpp - TeeRISC Target Descriptions -------------===//
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

#include "TeeRISCMCTargetDesc.h"
#include "TeeRISCMCAsmInfo.h"
#include "InstPrinter/TeeRISCInstPrinter.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "TeeRISCGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "TeeRISCGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "TeeRISCGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createTeeRISCMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitTeeRISCMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createTeeRISCMCRegisterInfo(StringRef TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitTeeRISCMCRegisterInfo(X, /*Return Value*/TeeRISC::R1);
  return X;
}

static MCSubtargetInfo *createTeeRISCMCSubtargetInfo(StringRef TT, StringRef CPU,
                                                   StringRef FS) {
    MCSubtargetInfo *X = new MCSubtargetInfo();
    InitTeeRISCMCSubtargetInfo(X, TT, CPU, FS);
    return X;
}

// Code models. Some only make sense for 64-bit code.
//
// SunCC  Reloc   CodeModel  Constraints
// abs32  Static  Small      text+data+bss linked below 2^32 bytes
// abs44  Static  Medium     text+data+bss linked below 2^44 bytes
// abs64  Static  Large      text smaller than 2^31 bytes
// pic13  PIC_    Small      GOT < 2^13 bytes
// pic32  PIC_    Medium     GOT < 2^32 bytes
//
// All code models require that the text segment is smaller than 2GB.

static MCCodeGenInfo *createTeeRISCMCCodeGenInfo(StringRef TT, Reloc::Model RM,
                                               CodeModel::Model CM,
                                               CodeGenOpt::Level OL) {
  MCCodeGenInfo *X = new MCCodeGenInfo();

  // The default 32-bit code model is abs32/pic32.
  if (CM == CodeModel::Default)
    CM = RM == Reloc::PIC_ ? CodeModel::Medium : CodeModel::Small;

  X->InitMCCodeGenInfo(RM, CM, OL);
  return X;
}

static MCStreamer *createMCStreamer(const Target &T, StringRef TT,
                                    MCContext &Ctx, MCAsmBackend &MAB,
                                    raw_ostream &_OS,
                                    MCCodeEmitter *_Emitter,
                                    bool RelaxAll,
                                    bool NoExecStack) {
  Triple TheTriple(TT);

  return createELFStreamer(Ctx, MAB, _OS, _Emitter, RelaxAll, NoExecStack);
}

static MCInstPrinter *createTeeRISCMCInstPrinter(const Target &T,
                                                 unsigned SyntaxVariant,
                                                 const MCAsmInfo &MAI,
                                                 const MCInstrInfo &MII,
                                                 const MCRegisterInfo &MRI,
                                                 const MCSubtargetInfo &STI) {
  if (SyntaxVariant == 0)
    return new TeeRISCInstPrinter(MAI, MII, MRI);
  return 0;
}

extern "C" void LLVMInitializeTeeRISCTargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfo<TeeRISCELFMCAsmInfo> X(TheTeeRISCTarget);

  // Register the MC codegen info.
  TargetRegistry::RegisterMCCodeGenInfo(TheTeeRISCTarget, createTeeRISCMCCodeGenInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheTeeRISCTarget, createTeeRISCMCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheTeeRISCTarget, createTeeRISCMCRegisterInfo);

  // Register the MC Code Emitter
  TargetRegistry::RegisterMCCodeEmitter(TheTeeRISCTarget, createTeeRISCMCCodeEmitter);

  // Register the object streamer.
  TargetRegistry::RegisterMCObjectStreamer(TheTeeRISCTarget, createMCStreamer);

  // Register the asm backend.
  TargetRegistry::RegisterMCAsmBackend(TheTeeRISCTarget,  createTeeRISCAsmBackend);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheTeeRISCTarget, createTeeRISCMCSubtargetInfo);

    // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(TheTeeRISCTarget, createTeeRISCMCInstPrinter);
}

