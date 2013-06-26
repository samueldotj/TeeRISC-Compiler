//===- TeeRISCMachineFunctionInfo.h - TeeRISC Machine Function Info -*- C++ *-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares  TeeRISC specific per-machine-function information.
//
//===----------------------------------------------------------------------===//
#ifndef TeeRISCMACHINEFUNCTIONINFO_H
#define TeeRISCMACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

  class TeeRISCMachineFunctionInfo : public MachineFunctionInfo {
    virtual void anchor();
  private:
    /// Holds for each function where on the stack the Frame Pointer must be
    /// saved. This is used on Prologue and Epilogue to emit FP save/restore
    int FPStackOffset;

    /// Holds for each function where on the stack the Return Address must be
    /// saved. This is used on Prologue and Epilogue to emit RA save/restore
    int RAStackOffset;

    /// TeeRISCFIHolder - Holds a FrameIndex and it's Stack Pointer Offset
    struct TeeRISCFIHolder {

      int FI;
      int SPOffset;

      TeeRISCFIHolder(int FrameIndex, int StackPointerOffset)
       : FI(FrameIndex), SPOffset(StackPointerOffset) {}
    };

    /// On LowerFormalArguments the stack size is unknown, so the Stack
    /// Pointer Offset calculation of "not in register arguments" must be
    /// postponed to emitPrologue.
    SmallVector<TeeRISCFIHolder, 16> FnLoadArgs;
    bool HasLoadArgs;

    // When VarArgs, we must write registers back to caller stack, preserving
    // on register arguments. Since the stack size is unknown on
    // LowerFormalArguments, the Stack Pointer Offset calculation must be
    // postponed to emitPrologue.
    SmallVector<TeeRISCFIHolder, 4> FnStoreVarArgs;
    bool HasStoreVarArgs;

    /// GlobalBaseReg - keeps track of the virtual register initialized for
    /// use as the global base register. This is used for PIC in some PIC
    /// relocation models.
    unsigned GlobalBaseReg;

    // VarArgsFrameIndex - FrameIndex for start of varargs area.
    int VarArgsFrameIndex;

    /// SRetReturnReg - Holds the virtual register into which the sret
    /// argument is passed.
    unsigned SRetReturnReg;

    /// LiveInFI - keeps track of the frame indexes in a callers stack
    /// frame that are live into a function.
    SmallVector<int, 16> LiveInFI;
  public:
    TeeRISCMachineFunctionInfo()
      : FPStackOffset(0), RAStackOffset(0), HasLoadArgs(false), HasStoreVarArgs(false),
        GlobalBaseReg(0), VarArgsFrameIndex(0), SRetReturnReg(0), LiveInFI() {}
    explicit TeeRISCMachineFunctionInfo(MachineFunction &MF)
      : FPStackOffset(0), RAStackOffset(0), HasLoadArgs(false), HasStoreVarArgs(false),
        GlobalBaseReg(0), VarArgsFrameIndex(0), SRetReturnReg(0), LiveInFI() {}

    int getFPStackOffset() const { return FPStackOffset; }
    void setFPStackOffset(int Off) { FPStackOffset = Off; }

    int getRAStackOffset() const { return RAStackOffset; }
    void setRAStackOffset(int Off) { RAStackOffset = Off; }

    unsigned getGlobalBaseReg() const { return GlobalBaseReg; };
    void setGlobalBaseReg(unsigned Reg) { GlobalBaseReg = Reg; }

    unsigned getSRetReturnReg() const { return SRetReturnReg; }
    void setSRetReturnReg(unsigned Reg) { SRetReturnReg = Reg; }

    bool hasLoadArgs() const { return HasLoadArgs; }
    bool hasStoreVarArgs() const { return HasStoreVarArgs; }

    void recordLiveIn(int FI) {
      LiveInFI.push_back(FI);
    }

    bool isLiveIn(int FI) {
    for (unsigned i = 0, e = LiveInFI.size(); i < e; ++i)
      if (FI == LiveInFI[i]) return true;

      return false;
    }

    const SmallVector<int, 16>& getLiveIn() const { 
      return LiveInFI; 
    }

    void recordLoadArgsFI(int FI, int SPOffset) {
      if (!HasLoadArgs) HasLoadArgs=true;
      FnLoadArgs.push_back(TeeRISCFIHolder(FI, SPOffset));
    }

    void recordStoreVarArgsFI(int FI, int SPOffset) {
      if (!HasStoreVarArgs) HasStoreVarArgs=true;
      FnStoreVarArgs.push_back(TeeRISCFIHolder(FI, SPOffset));
    }

    void adjustLoadArgsFI(MachineFrameInfo *MFI) const {
      if (!hasLoadArgs()) return;
      for (unsigned i = 0, e = FnLoadArgs.size(); i != e; ++i)
        MFI->setObjectOffset(FnLoadArgs[i].FI, FnLoadArgs[i].SPOffset);
    }

    void adjustStoreVarArgsFI(MachineFrameInfo *MFI) const {
      if (!hasStoreVarArgs()) return;
      for (unsigned i = 0, e = FnStoreVarArgs.size(); i != e; ++i)
        MFI->setObjectOffset(FnStoreVarArgs[i].FI, FnStoreVarArgs[i].SPOffset);
    }

    int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
    void setVarArgsFrameIndex(int Index) { VarArgsFrameIndex = Index; }
  };
}

#endif
