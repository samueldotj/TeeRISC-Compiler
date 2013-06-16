//===-- TeeRISCSelectionDAGInfo.cpp - TeeRISC SelectionDAG Info -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the TeeRISCSelectionDAGInfo class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "TeeRISC-selectiondag-info"
#include "TeeRISCTargetMachine.h"
using namespace llvm;

TeeRISCSelectionDAGInfo::TeeRISCSelectionDAGInfo(const TeeRISCTargetMachine &TM)
  : TargetSelectionDAGInfo(TM) {
}

TeeRISCSelectionDAGInfo::~TeeRISCSelectionDAGInfo() {
}
