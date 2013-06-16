//===-- TeeRISCRegisterInfo.h - TeeRISC Register Information Impl ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the TeeRISC implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef TeeRISCREGISTERINFO_H
#define TeeRISCREGISTERINFO_H

#include "llvm/Target/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "TeeRISCGenRegisterInfo.inc"

namespace llvm {

class TeeRISCSubtarget;
class TargetInstrInfo;
class Type;

struct TeeRISCRegisterInfo : public TeeRISCGenRegisterInfo {
  TeeRISCSubtarget &Subtarget;
  const TargetInstrInfo &TII;

  TeeRISCRegisterInfo(TeeRISCSubtarget &st, const TargetInstrInfo &tii);

  /// Code Generation virtual methods...
  const uint16_t *getCalleeSavedRegs(const MachineFunction *MF = 0) const;

  BitVector getReservedRegs(const MachineFunction &MF) const;

  const TargetRegisterClass *getPointerRegClass(const MachineFunction &MF,
                                                unsigned Kind) const;

  void eliminateFrameIndex(MachineBasicBlock::iterator II,
                           int SPAdj, unsigned FIOperandNum,
                           RegScavenger *RS = NULL) const;

  void processFunctionBeforeFrameFinalized(MachineFunction &MF,
                                       RegScavenger *RS = NULL) const;

  // Debug information queries.
  unsigned getFrameRegister(const MachineFunction &MF) const;

  // Exception handling queries.
  unsigned getEHExceptionRegister() const;
  unsigned getEHHandlerRegister() const;
};

} // end namespace llvm

#endif
