//===-- TeeRISCTargetMachine.h - Define TargetMachine for TeeRISC ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the TeeRISC specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef TeeRISCTARGETMACHINE_H
#define TeeRISCTARGETMACHINE_H

#if 0
#include "TeeRISCFrameLowering.h"
#include "TeeRISCISelLowering.h"
#include "TeeRISCInstrInfo.h"
#include "TeeRISCSelectionDAGInfo.h"
#include "TeeRISCSubtarget.h"
#endif
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class TeeRISCTargetMachine : public LLVMTargetMachine {
  TeeRISCSubtarget Subtarget;
  const DataLayout DL;       // Calculates type size & alignment
  TeeRISCInstrInfo InstrInfo;
  TeeRISCTargetLowering TLInfo;
  TeeRISCSelectionDAGInfo TSInfo;
  TeeRISCFrameLowering FrameLowering;
public:
  TeeRISCTargetMachine(const Target &T, StringRef TT,
                     StringRef CPU, StringRef FS, const TargetOptions &Options,
                     Reloc::Model RM, CodeModel::Model CM,
                     CodeGenOpt::Level OL, bool is64bit);

  virtual const TeeRISCInstrInfo *getInstrInfo() const { return &InstrInfo; }
  virtual const TargetFrameLowering  *getFrameLowering() const {
    return &FrameLowering;
  }
  virtual const TeeRISCSubtarget   *getSubtargetImpl() const{ return &Subtarget; }
  virtual const TeeRISCRegisterInfo *getRegisterInfo() const {
    return &InstrInfo.getRegisterInfo();
  }
  virtual const TeeRISCTargetLowering* getTargetLowering() const {
    return &TLInfo;
  }
  virtual const TeeRISCSelectionDAGInfo* getSelectionDAGInfo() const {
    return &TSInfo;
  }
  virtual const DataLayout       *getDataLayout() const { return &DL; }

  // Pass Pipeline Configuration
  virtual TargetPassConfig *createPassConfig(PassManagerBase &PM);
};

/// TeeRISCTargetMachine - TeeRISC 32-bit target machine
///
class TeeRISCTargetMachine : public TeeRISCTargetMachine {
  virtual void anchor();
public:
  TeeRISCTargetMachine(const Target &T, StringRef TT,
                       StringRef CPU, StringRef FS,
                       const TargetOptions &Options,
                       Reloc::Model RM, CodeModel::Model CM,
                       CodeGenOpt::Level OL);
};

} // end namespace llvm

#endif
