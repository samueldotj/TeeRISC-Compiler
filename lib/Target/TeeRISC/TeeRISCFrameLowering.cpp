//===-- TeeRISCFrameLowering.cpp - TeeRISC Frame Information --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the TeeRISC implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "TeeRISCFrameLowering.h"
#include "TeeRISCInstrInfo.h"
#include "TeeRISCMachineFunctionInfo.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

void TeeRISCFrameLowering::emitPrologue(MachineFunction &MF) const {
  MachineBasicBlock &MBB = MF.front();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  const TeeRISCInstrInfo &TII =
    *static_cast<const TeeRISCInstrInfo*>(MF.getTarget().getInstrInfo());
  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  // Get the number of bytes to allocate from the FrameInfo
  int NumBytes = (int) MFI->getStackSize();

  // Emit the correct save instruction based on the number of bytes in
  // the frame. Minimum stack frame size according to V8 ABI is:
  //   16 words for register window spill
  //    1 word for address of returned aggregate-value
  // +  6 words for passing parameters on the stack
  // ----------
  //   23 words * 4 bytes per word = 92 bytes
  NumBytes += 92;

  // Round up to next doubleword boundary -- a double-word boundary
  // is required by the ABI.
  NumBytes = RoundUpToAlignment(NumBytes, 8);
  NumBytes = -NumBytes;

  if (NumBytes >= -4096) {
    BuildMI(MBB, MBBI, dl, TII.get(TeeRISC::ADD), TeeRISC::SP)
      .addReg(TeeRISC::SP).addImm(NumBytes);
  } else {
#if 0
    // Emit this the hard way.  This clobbers R11 which we always know is
    // available here.
    unsigned OffHi = (unsigned)NumBytes >> 16U;
    BuildMI(MBB, MBBI, dl, TII.get(::ORimm), TeeRISC::R11).addImm(OffHi);
    // Emit R11 = R11 + I6
    BuildMI(MBB, MBBI, dl, TII.get(::ORimm), TeeRISC::R11)
      .addReg(::R11).addImm(NumBytes & ((1 << 16)-1));
    BuildMI(MBB, MBBI, dl, TII.get(::SAVErr), TeeRISC::SP)
      .addReg(::SP).addReg(TeeRISC::R11);
#endif
    assert(0 && "NOT implemented yet");
  }
}

void TeeRISCFrameLowering::
eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator I) const {
}


void TeeRISCFrameLowering::emitEpilogue(MachineFunction &MF,
                                        MachineBasicBlock &MBB) const {
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  MachineFrameInfo *MFI            = MF.getFrameInfo();
  const TeeRISCInstrInfo &TII      =
    *static_cast<const TeeRISCInstrInfo*>(MF.getTarget().getInstrInfo());
  DebugLoc dl                       = MBBI->getDebugLoc();
  int StackSize                     = (int) MFI->getStackSize();

#if 0
  assert(MBBI->getOpcode() == TeeRISC::RET && "Can only put epilog before 'retl' instruction!");
  if (StackSize) {
      BuildMI(MBB, MBBI, dl, TII.get(TeeRISC::ORimm), TeeRISC::R11)
          addReg(TeeRISC::ZERO).addImm(StackSize);
      BuildMI(MBB, MBBI, dl, TII.get(TeeRISC::ADD), 
              TeeRISC::SP).addReg(TeeRISC::R11).addReg(TeeRISC::ZERO);
  }
#endif
}

bool TeeRISCFrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
  //Reserve call frame if there are no variable sized objects on the stack
  return !MF.getFrameInfo()->hasVarSizedObjects();
}

// hasFP - Return true if the specified function should have a dedicated frame
// pointer register.  This is true if the function has variable sized allocas or
// if frame pointer elimination is disabled.
bool TeeRISCFrameLowering::hasFP(const MachineFunction &MF) const {
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  return MF.getTarget().Options.DisableFramePointerElim(MF) ||
    MFI->hasVarSizedObjects() || MFI->isFrameAddressTaken();
}

