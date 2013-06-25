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
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static void determineFrameLayout(MachineFunction &MF) {
  MachineFrameInfo *MFI = MF.getFrameInfo();
  TeeRISCMachineFunctionInfo *TFI = MF.getInfo<TeeRISCMachineFunctionInfo>();

  // Replace the dummy '0' SPOffset by the negative offsets, as explained on
  // LowerFORMAL_ARGUMENTS. Leaving '0' for while is necessary to avoid
  // the approach done by calculateFrameObjectOffsets to the stack frame.
  TFI->adjustLoadArgsFI(MFI);
  TFI->adjustStoreVarArgsFI(MFI);

  // Get the number of bytes to allocate from the FrameInfo
  unsigned FrameSize = MFI->getStackSize();
  DEBUG(dbgs() << "Original Frame Size: " << FrameSize << "\n" );

  // Get the alignments provided by the target, and the maximum alignment
  // (if any) of the fixed frame objects.
  // unsigned MaxAlign = MFI->getMaxAlignment();
  unsigned TargetAlign = MF.getTarget().getFrameLowering()->getStackAlignment();
  unsigned AlignMask = TargetAlign - 1;

  // Make sure the frame is aligned.
  FrameSize = (FrameSize + AlignMask) & ~AlignMask;
  MFI->setStackSize(FrameSize);
  DEBUG(dbgs() << "Aligned Frame Size: " << FrameSize << "\n" );
}

void TeeRISCFrameLowering::emitPrologue(MachineFunction &MF) const {
  MachineBasicBlock &MBB   = MF.front();
  MachineFrameInfo *MFI    = MF.getFrameInfo();
  const TeeRISCInstrInfo &TII =
    *static_cast<const TeeRISCInstrInfo*>(MF.getTarget().getInstrInfo());
  TeeRISCMachineFunctionInfo *TFI = MF.getInfo<TeeRISCMachineFunctionInfo>();
  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  // Determine the correct frame layout
  determineFrameLayout(MF);

  // Get the number of bytes to allocate from the FrameInfo.
  unsigned StackSize = MFI->getStackSize();

  // No need to allocate space on the stack.
  if (StackSize == 0 && !MFI->adjustsStack()) return;

  int FPOffset = TFI->getFPStackOffset();
  int RAOffset = TFI->getRAStackOffset();

  // Adjust stack : SP = SP - StackSize
  BuildMI(MBB, MBBI, DL, TII.get(TeeRISC::ADD), TeeRISC::SP)
      .addReg(TeeRISC::SP).addImm(-StackSize);

  // *(SP + RAOffset) = LR 
  if (MFI->adjustsStack()) {
    BuildMI(MBB, MBBI, DL, TII.get(TeeRISC::ST))
        .addReg(TeeRISC::SP).addReg(TeeRISC::LR).addImm(RAOffset);
  }

  if (hasFP(MF)) {
    // *(SP + FPOffset) = FP
    BuildMI(MBB, MBBI, DL, TII.get(TeeRISC::ST))
      .addReg(TeeRISC::SP).addReg(TeeRISC::FP).addImm(FPOffset);

    // FP = SP
    BuildMI(MBB, MBBI, DL, TII.get(TeeRISC::ADD), TeeRISC::FP)
      .addReg(TeeRISC::SP).addReg(TeeRISC::ZERO);
  }
}

void TeeRISCFrameLowering::emitEpilogue(MachineFunction &MF,
                                        MachineBasicBlock &MBB) const {
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  MachineFrameInfo *MFI            = MF.getFrameInfo();
  TeeRISCMachineFunctionInfo *TFI  = MF.getInfo<TeeRISCMachineFunctionInfo>();
  const TeeRISCInstrInfo &TII =
    *static_cast<const TeeRISCInstrInfo*>(MF.getTarget().getInstrInfo());

  DebugLoc dl = MBBI->getDebugLoc();

  // Get the FI's where RA and FP are saved.
  int FPOffset = TFI->getFPStackOffset();
  int RAOffset = TFI->getRAStackOffset();

  if (hasFP(MF)) {
    // SP = FP 
    BuildMI(MBB, MBBI, dl, TII.get(TeeRISC::ADD), TeeRISC::SP)
      .addReg(TeeRISC::FP).addReg(TeeRISC::ZERO);

    // FP = *(SP + FPOffset)
    BuildMI(MBB, MBBI, dl, TII.get(TeeRISC::LD), TeeRISC::FP)
      .addReg(TeeRISC::SP).addImm(FPOffset);
  }

  // LR = *(R1 + RAOffset)
  if (MFI->adjustsStack()) {
    BuildMI(MBB, MBBI, dl, TII.get(TeeRISC::LD), TeeRISC::LR)
      .addReg(TeeRISC::SP).addImm(RAOffset);
  }

  // Get the number of bytes from FrameInfo
  int StackSize = (int) MFI->getStackSize();

  // SP = SP + StackSize
  if (StackSize) {
    BuildMI(MBB, MBBI, dl, TII.get(TeeRISC::ADD), TeeRISC::SP)
      .addReg(TeeRISC::SP).addImm(StackSize);
  }
}

void TeeRISCFrameLowering::
eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator I) const {
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


void TeeRISCFrameLowering::
processFunctionBeforeCalleeSavedScan(MachineFunction &MF,
                                     RegScavenger *RS) const {
  MachineFrameInfo *MFI = MF.getFrameInfo();
  TeeRISCMachineFunctionInfo *TFI = MF.getInfo<TeeRISCMachineFunctionInfo>();

  if (MFI->adjustsStack()) {
    TFI->setRAStackOffset(0);
    MFI->CreateFixedObject(4,0,true);
  }

  if (hasFP(MF)) {
    TFI->setFPStackOffset(4);
    MFI->CreateFixedObject(4,4,true);
  }

  //analyzeFrameIndexes(MF);
}

