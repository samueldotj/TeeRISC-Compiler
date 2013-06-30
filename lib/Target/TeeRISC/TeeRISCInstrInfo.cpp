//===-- TeeRISCInstrInfo.cpp - TeeRISC Instruction Information ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the TeeRISC implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "TeeRISCInstrInfo.h"
#include "TeeRISC.h"
#include "TeeRISCMachineFunctionInfo.h"
#include "TeeRISCSubtarget.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_CTOR
#include "TeeRISCGenInstrInfo.inc"

using namespace llvm;

TeeRISCInstrInfo::TeeRISCInstrInfo(TeeRISCSubtarget &ST)
  : TeeRISCGenInstrInfo(),
    RI(ST, *this), Subtarget(ST) {
}

/// isLoadFromStackSlot - If the specified machine instruction is a direct
/// load from a stack slot, return the virtual or physical register number of
/// the destination along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than loading from the stack slot.
/// Dest = *(Src + Imm)
///  Op0 =   Op1   Op2
unsigned TeeRISCInstrInfo::isLoadFromStackSlot(const MachineInstr *MI,
                                               int &FrameIndex) const {
  assert(0);
  if (MI->getOpcode() == TeeRISC::LD) {
    if (MI->getOperand(1).isFI() && MI->getOperand(2).isImm() &&
        MI->getOperand(2).getImm() == 0) {
      FrameIndex = MI->getOperand(1).getIndex();
      return MI->getOperand(0).getReg();
    }
  }
  return 0;
}

/// isStoreToStackSlot - If the specified machine instruction is a direct
/// store to a stack slot, return the virtual or physical register number of
/// the source reg along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than storing to the stack slot.
/// *(Dest + Imm) = Src
///    Op0 + Op1    Op2
unsigned TeeRISCInstrInfo::isStoreToStackSlot(const MachineInstr *MI,
                                              int &FrameIndex) const {

  assert(0);
  if (MI->getOpcode() == TeeRISC::ST) {
    if (MI->getOperand(1).isFI() && MI->getOperand(2).isImm() &&
        MI->getOperand(2).getImm() == 0) {
      FrameIndex = MI->getOperand(1).getIndex();
      return MI->getOperand(0).getReg();
    }
  }
  return 0;
}

static bool IsIntegerCC(unsigned CC)
{
  return  (CC <= TeeRISCCC::ICC_VC);
}


static TeeRISCCC::CondCodes GetOppositeBranchCondition(TeeRISCCC::CondCodes CC)
{
  switch(CC) {
  case TeeRISCCC::ICC_NE:   return TeeRISCCC::ICC_E;
  case TeeRISCCC::ICC_E:    return TeeRISCCC::ICC_NE;
  case TeeRISCCC::ICC_G:    return TeeRISCCC::ICC_LE;
  case TeeRISCCC::ICC_LE:   return TeeRISCCC::ICC_G;
  case TeeRISCCC::ICC_GE:   return TeeRISCCC::ICC_L;
  case TeeRISCCC::ICC_L:    return TeeRISCCC::ICC_GE;
  case TeeRISCCC::ICC_GU:   return TeeRISCCC::ICC_LEU;
  case TeeRISCCC::ICC_LEU:  return TeeRISCCC::ICC_GU;
  case TeeRISCCC::ICC_CC:   return TeeRISCCC::ICC_CS;
  case TeeRISCCC::ICC_CS:   return TeeRISCCC::ICC_CC;
  case TeeRISCCC::ICC_POS:  return TeeRISCCC::ICC_NEG;
  case TeeRISCCC::ICC_NEG:  return TeeRISCCC::ICC_POS;
  case TeeRISCCC::ICC_VC:   return TeeRISCCC::ICC_VS;
  case TeeRISCCC::ICC_VS:   return TeeRISCCC::ICC_VC;
  }
  llvm_unreachable("Invalid cond code");
}

MachineInstr *
TeeRISCInstrInfo::emitFrameIndexDebugValue(MachineFunction &MF,
                                         int FrameIx,
                                         uint64_t Offset,
                                         const MDNode *MDPtr,
                                         DebugLoc dl) const {
  MachineInstrBuilder MIB = BuildMI(MF, dl, get(TeeRISC::DBG_VALUE))
    .addFrameIndex(FrameIx).addImm(0).addImm(Offset).addMetadata(MDPtr);
  return &*MIB;
}


bool TeeRISCInstrInfo::AnalyzeBranch(MachineBasicBlock &MBB,
                                   MachineBasicBlock *&TBB,
                                   MachineBasicBlock *&FBB,
                                   SmallVectorImpl<MachineOperand> &Cond,
                                   bool AllowModify) const
{

  MachineBasicBlock::iterator I = MBB.end();
  MachineBasicBlock::iterator UnCondBrIter = MBB.end();
  while (I != MBB.begin()) {
    --I;

    if (I->isDebugValue())
      continue;

    //When we see a non-terminator, we are done
    if (!isUnpredicatedTerminator(I))
      break;

    //Terminator is not a branch
    if (!I->isBranch())
      return true;

    //Handle Unconditional branches
    if (I->getOpcode() == TeeRISC::CALL) {
      UnCondBrIter = I;

      if (!AllowModify) {
        TBB = I->getOperand(0).getMBB();
        continue;
      }

      while (llvm::next(I) != MBB.end())
        llvm::next(I)->eraseFromParent();

      Cond.clear();
      FBB = 0;

      if (MBB.isLayoutSuccessor(I->getOperand(0).getMBB())) {
        TBB = 0;
        I->eraseFromParent();
        I = MBB.end();
        UnCondBrIter = MBB.end();
        continue;
      }

      TBB = I->getOperand(0).getMBB();
      continue;
    }

    unsigned Opcode = I->getOpcode();
    if (Opcode != TeeRISC::CALL)
      return true; //Unknown Opcode

    TeeRISCCC::CondCodes BranchCode = (TeeRISCCC::CondCodes)I->getOperand(1).getImm();

    if (Cond.empty()) {
      MachineBasicBlock *TargetBB = I->getOperand(0).getMBB();
      if (AllowModify && UnCondBrIter != MBB.end() &&
          MBB.isLayoutSuccessor(TargetBB)) {

        //Transform the code
        //
        //    brCC L1
        //    ba L2
        // L1:
        //    ..
        // L2:
        //
        // into
        //
        //   brnCC L2
        // L1:
        //   ...
        // L2:
        //
        BranchCode = GetOppositeBranchCondition(BranchCode);
        MachineBasicBlock::iterator OldInst = I;
        BuildMI(MBB, UnCondBrIter, MBB.findDebugLoc(I), get(Opcode))
          .addMBB(UnCondBrIter->getOperand(0).getMBB()).addImm(BranchCode);
        BuildMI(MBB, UnCondBrIter, MBB.findDebugLoc(I), get(TeeRISC::CALL))
          .addMBB(TargetBB);

        OldInst->eraseFromParent();
        UnCondBrIter->eraseFromParent();

        UnCondBrIter = MBB.end();
        I = MBB.end();
        continue;
      }
      FBB = TBB;
      TBB = I->getOperand(0).getMBB();
      Cond.push_back(MachineOperand::CreateImm(BranchCode));
      continue;
    }
    //FIXME: Handle subsequent conditional branches
    //For now, we can't handle multiple conditional branches
    return true;
  }
  return false;
}

unsigned
TeeRISCInstrInfo::InsertBranch(MachineBasicBlock &MBB,MachineBasicBlock *TBB,
                             MachineBasicBlock *FBB,
                             const SmallVectorImpl<MachineOperand> &Cond,
                             DebugLoc DL) const {
  assert(TBB && "InsertBranch must not be told to insert a fallthrough");
  assert((Cond.size() == 1 || Cond.size() == 0) &&
         "TeeRISC branch conditions should have one component!");

  if (Cond.empty()) {
    assert(!FBB && "Unconditional branch with multiple successors!");
    BuildMI(&MBB, DL, get(TeeRISC::CALL)).addMBB(TBB);
    return 1;
  }

  //Conditional branch
  unsigned CC = Cond[0].getImm();

  if (IsIntegerCC(CC))
    BuildMI(&MBB, DL, get(TeeRISC::CALL)).addMBB(TBB).addImm(CC);
  if (!FBB)
    return 1;

  BuildMI(&MBB, DL, get(TeeRISC::CALL)).addMBB(FBB);
  return 2;
}

unsigned TeeRISCInstrInfo::RemoveBranch(MachineBasicBlock &MBB) const
{
  MachineBasicBlock::iterator I = MBB.end();
  unsigned Count = 0;
  while (I != MBB.begin()) {
    --I;

    if (I->isDebugValue())
      continue;

    if (I->getOpcode() != TeeRISC::CALL)
      break; // Not a branch

    I->eraseFromParent();
    I = MBB.end();
    ++Count;
  }
  return Count;
}

void TeeRISCInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator I, DebugLoc DL,
                                 unsigned DestReg, unsigned SrcReg,
                                 bool KillSrc) const {
  if (TeeRISC::IntRegsRegClass.contains(DestReg, SrcReg))
    BuildMI(MBB, I, DL, get(TeeRISC::OR), DestReg).addReg(TeeRISC::ZERO)
      .addReg(SrcReg, getKillRegState(KillSrc));
  else
    llvm_unreachable("Impossible reg-to-reg copy");
}

void TeeRISCInstrInfo::
storeRegToStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                    unsigned SrcReg, bool isKill, int FI,
                    const TargetRegisterClass *RC,
                    const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end()) DL = I->getDebugLoc();

  // On the order of operands here: think "[FrameIdx + 0] = SrcReg".
  if (RC == &TeeRISC::IntRegsRegClass)
    BuildMI(MBB, I, DL, get(TeeRISC::ST)).addFrameIndex(FI).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
  else
    llvm_unreachable("Can't store this register to stack slot");
}

void TeeRISCInstrInfo::
loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                     unsigned DestReg, int FI,
                     const TargetRegisterClass *RC,
                     const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end()) DL = I->getDebugLoc();

  if (RC == &TeeRISC::IntRegsRegClass)
    BuildMI(MBB, I, DL, get(TeeRISC::LD), DestReg).addFrameIndex(FI).addImm(0);
  else
    llvm_unreachable("Can't load this register from stack slot");
}

unsigned TeeRISCInstrInfo::getGlobalBaseReg(MachineFunction *MF) const
{
  TeeRISCMachineFunctionInfo *TeeRISCFI = MF->getInfo<TeeRISCMachineFunctionInfo>();
  unsigned GlobalBaseReg = TeeRISCFI->getGlobalBaseReg();

  if (GlobalBaseReg != 0)
    return GlobalBaseReg;

  // Insert the set of GlobalBaseReg into the first MBB of the function
  MachineBasicBlock &FirstMBB = MF->front();
  MachineBasicBlock::iterator MBBI = FirstMBB.begin();
  MachineRegisterInfo &RegInfo = MF->getRegInfo();

  GlobalBaseReg = RegInfo.createVirtualRegister(&TeeRISC::IntRegsRegClass);

  DebugLoc dl;

  BuildMI(FirstMBB, MBBI, dl, get(TeeRISC::ADD), GlobalBaseReg).addReg(TeeRISC::R10);
  RegInfo.addLiveIn(TeeRISC::R10);
  TeeRISCFI->setGlobalBaseReg(GlobalBaseReg);
  return GlobalBaseReg;
}

