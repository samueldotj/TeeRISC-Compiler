//===-- TeeRISCRegisterInfo.cpp - TeeRISC Register Information ------------===//
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

#include "TeeRISCRegisterInfo.h"
#include "TeeRISC.h"
#include "TeeRISCSubtarget.h"
#include "TeeRISCMachineFunctionInfo.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetInstrInfo.h"

#define GET_REGINFO_TARGET_DESC
#include "TeeRISCGenRegisterInfo.inc"
#include "TeeRISCMachineFunctionInfo.h"

using namespace llvm;

TeeRISCRegisterInfo::TeeRISCRegisterInfo(TeeRISCSubtarget &st,
                                         const TargetInstrInfo &tii)
  : TeeRISCGenRegisterInfo(TeeRISC::IP), Subtarget(st), TII(tii) {
}

const uint16_t* TeeRISCRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF)
                                                                         const {
  static const uint16_t CalleeSavedRegs[] = {
    TeeRISC::R6, TeeRISC::R7, TeeRISC::R8, TeeRISC::R9, TeeRISC::R10
  };
  return CalleeSavedRegs;
}

BitVector TeeRISCRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  static const uint16_t ReservedCPURegs[] = {
    TeeRISC::R0,  // ZERO
    TeeRISC::R1,  // RETURN VALUE
    TeeRISC::R14, // FP
    TeeRISC::R15, // SP

    TeeRISC::R16, // IP
    TeeRISC::R17, // LR
    TeeRISC::R18, // E_LR
    TeeRISC::R19, // I_LR
    TeeRISC::R20, // FLAGS
    TeeRISC::R21, // S_FLAGS
    TeeRISC::R22, // CR0
    TeeRISC::R23,
    TeeRISC::R24,
    TeeRISC::R25,
    TeeRISC::R26,
    TeeRISC::R27,
    TeeRISC::R28,
    TeeRISC::R29,
    TeeRISC::R30,
    TeeRISC::R31
  };

  BitVector Reserved(getNumRegs());
  typedef TargetRegisterClass::iterator RegIter;

  for (unsigned I = 0; I < array_lengthof(ReservedCPURegs); ++I)
    Reserved.set(ReservedCPURegs[I]);

  return Reserved;
}

const TargetRegisterClass*
TeeRISCRegisterInfo::getPointerRegClass(const MachineFunction &MF,
                                        unsigned Kind) const {
  return &TeeRISC::IntRegsRegClass;
}

// FrameIndex represent objects inside a abstract stack.
// We must replace FrameIndex with an stack/frame pointer
// direct reference.
void
TeeRISCRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                         int SPAdj, unsigned FIOperandNum,
                                         RegScavenger *RS) const {
  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  unsigned OFIOperandNum = FIOperandNum + 1;

  DEBUG(dbgs() << "\nFunction : " << MF.getName() << "\n";
        dbgs() << "<--------->\n" << MI);
  
  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
  int stackSize  = MFI->getStackSize();
  int spOffset   = MFI->getObjectOffset(FrameIndex);

  DEBUG(TeeRISCMachineFunctionInfo *TFI = MF.getInfo<TeeRISCMachineFunctionInfo>();
        dbgs() << "eliminateFrameIndex" << "\n"
               << "FrameIndex : " << FrameIndex << "\n"
               << "spOffset   : " << spOffset << "\n"
               << "stackSize  : " << stackSize << "\n"
               << "isFixed    : " << MFI->isFixedObjectIndex(FrameIndex) << "\n"
               << "isLiveIn   : " << TFI->isLiveIn(FrameIndex) << "\n"
               << "isSpill    : " << MFI->isSpillSlotObjectIndex(FrameIndex)
               << "\n" );

  // as explained on LowerFormalArguments, detect negative offsets
  // and adjust SPOffsets considering the final stack size.
  int Offset = (spOffset < 0) ? (stackSize - spOffset) : spOffset;
  Offset += MI.getOperand(OFIOperandNum).getImm();

  // Replace frame index with a frame pointer reference.
  MI.getOperand(OFIOperandNum).ChangeToImmediate(Offset);
  MI.getOperand(FIOperandNum).ChangeToRegister(getFrameRegister(MF), false);
}

unsigned TeeRISCRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();

  return TFI->hasFP(MF) ? TeeRISC::FP : TeeRISC::SP;
}

unsigned TeeRISCRegisterInfo::getEHExceptionRegister() const {
  llvm_unreachable("What is the exception register");
}

unsigned TeeRISCRegisterInfo::getEHHandlerRegister() const {
  llvm_unreachable("What is the exception handler register");
}
