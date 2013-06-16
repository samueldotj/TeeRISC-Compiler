//===-- TeeRISCSelectionDAGInfo.h - TeeRISC SelectionDAG Info ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the TeeRISC subclass for TargetSelectionDAGInfo.
//
//===----------------------------------------------------------------------===//

#ifndef TeeRISCSELECTIONDAGINFO_H
#define TeeRISCSELECTIONDAGINFO_H

#include "llvm/Target/TargetSelectionDAGInfo.h"

namespace llvm {

class TeeRISCTargetMachine;

class TeeRISCSelectionDAGInfo : public TargetSelectionDAGInfo {
public:
  explicit TeeRISCSelectionDAGInfo(const TeeRISCTargetMachine &TM);
  ~TeeRISCSelectionDAGInfo();
};

}

#endif
