//===-- TeeRISCFrameLowering.h - Define frame lowering for TeeRISC -*- C++ -*=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef TeeRISC_FRAMEINFO_H
#define TeeRISC_FRAMEINFO_H

#include "TeeRISC.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
  class TeeRISCSubtarget;

class TeeRISCFrameLowering : public TargetFrameLowering {
  const TeeRISCSubtarget &SubTarget;
public:
  explicit TeeRISCFrameLowering(const TeeRISCSubtarget &ST)
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 8, 0, 8),
      SubTarget(ST) {}

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF) const;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const;

  void eliminateCallFramePseudoInstr(MachineFunction &MF,
                                     MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I) const;

  bool hasReservedCallFrame(const MachineFunction &MF) const;
  bool hasFP(const MachineFunction &MF) const;
};

} // End llvm namespace

#endif
