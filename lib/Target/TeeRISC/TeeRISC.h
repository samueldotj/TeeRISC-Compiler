//===-- TeeRISC.h - Top-level interface for TeeRISC representation -*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// TeeRISC back-end.
//
//===----------------------------------------------------------------------===//

#ifndef TARGET_TeeRISC_H
#define TARGET_TeeRISC_H

#include "MCTargetDesc/TeeRISCMCTargetDesc.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class FunctionPass;
  class TeeRISCTargetMachine;
  class formatted_raw_ostream;

  FunctionPass *createTeeRISCISelDag(TeeRISCTargetMachine &TM);
  FunctionPass *createTeeRISCDelaySlotFillerPass(TargetMachine &TM);
  FunctionPass *createTeeRISCFPMoverPass(TargetMachine &TM);

} // end namespace llvm;

namespace llvm {
  // Enums corresponding to TeeRISC condition codes, both icc's and fcc's.  These
  // values must be kept in sync with the ones in the .td file.
  namespace TeeRISCCC {
    enum CondCodes {
      //ICC_A   =  8   ,  // Always
      //ICC_N   =  0   ,  // Never
      ICC_NE  =  9   ,  // Not Equal
      ICC_E   =  1   ,  // Equal
      ICC_G   = 10   ,  // Greater
      ICC_LE  =  2   ,  // Less or Equal
      ICC_GE  = 11   ,  // Greater or Equal
      ICC_L   =  3   ,  // Less
      ICC_GU  = 12   ,  // Greater Unsigned
      ICC_LEU =  4   ,  // Less or Equal Unsigned
      ICC_CC  = 13   ,  // Carry Clear/Great or Equal Unsigned
      ICC_CS  =  5   ,  // Carry Set/Less Unsigned
      ICC_POS = 14   ,  // Positive
      ICC_NEG =  6   ,  // Negative
      ICC_VC  = 15   ,  // Overflow Clear
      ICC_VS  =  7      // Overflow Set
    };
  }
  
  inline static const char *TeeRISCCondCodeToString(TeeRISCCC::CondCodes CC) {
    switch (CC) {
    case TeeRISCCC::ICC_NE:  return "ne";
    case TeeRISCCC::ICC_E:   return "e";
    case TeeRISCCC::ICC_G:   return "g";
    case TeeRISCCC::ICC_LE:  return "le";
    case TeeRISCCC::ICC_GE:  return "ge";
    case TeeRISCCC::ICC_L:   return "l";
    case TeeRISCCC::ICC_GU:  return "gu";
    case TeeRISCCC::ICC_LEU: return "leu";
    case TeeRISCCC::ICC_CC:  return "cc";
    case TeeRISCCC::ICC_CS:  return "cs";
    case TeeRISCCC::ICC_POS: return "pos";
    case TeeRISCCC::ICC_NEG: return "neg";
    case TeeRISCCC::ICC_VC:  return "vc";
    case TeeRISCCC::ICC_VS:  return "vs";
    }
    llvm_unreachable("Invalid cond code");
  }
}  // end namespace llvm
#endif
