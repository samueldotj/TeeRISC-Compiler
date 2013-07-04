//===-- TeeRISCMCCodeEmitter.cpp - Convert TeeRISC code to machine code -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the TeeRISCMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "mccodeemitter"
#include "MCTargetDesc/TeeRISCMCTargetDesc.h"
#include "MCTargetDesc/TeeRISCBaseInfo.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

STATISTIC(MCNumEmitted, "Number of MC instructions emitted");

namespace {
class TeeRISCMCCodeEmitter : public MCCodeEmitter {
  TeeRISCMCCodeEmitter(const TeeRISCMCCodeEmitter &) LLVM_DELETED_FUNCTION;
  void operator=(const TeeRISCMCCodeEmitter &) LLVM_DELETED_FUNCTION;
  const MCInstrInfo &MCII;

public:
  TeeRISCMCCodeEmitter(const MCInstrInfo &mcii, const MCSubtargetInfo &sti,
                      MCContext &ctx): MCII(mcii) {
  }

  ~TeeRISCMCCodeEmitter() {}

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI) const;

  /// getMachineOpValue - Return binary encoding of operand. If the machine
  /// operand requires relocation, record the relocation and return zero.
  unsigned getMachineOpValue(const MCInst &MI,const MCOperand &MO) const;
  unsigned getMachineOpValue(const MCInst &MI, unsigned OpIdx) const {
    return getMachineOpValue(MI, MI.getOperand(OpIdx));
  }

  static unsigned GetTeeRISCRegNum(const MCOperand &MO) {
    // FIXME: getTeeRISCRegisterNumbering() is sufficient?
    llvm_unreachable("TeeRISCMCCodeEmitter::GetTeeRISCRegNum() not yet implemented.");
  }

  void EncodeInstruction(const MCInst &MI, raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups) const;
};

} // end anonymous namespace


MCCodeEmitter *llvm::createTeeRISCMCCodeEmitter(const MCInstrInfo &MCII,
                                               const MCRegisterInfo &MRI,
                                               const MCSubtargetInfo &STI,
                                               MCContext &Ctx) {
  return new TeeRISCMCCodeEmitter(MCII, STI, Ctx);
}

/// getMachineOpValue - Return binary encoding of operand. If the machine
/// operand requires relocation, record the relocation and return zero.
unsigned TeeRISCMCCodeEmitter::getMachineOpValue(const MCInst &MI,
                                             const MCOperand &MO) const {
  if (MO.isReg())
    return getTeeRISCRegisterNumbering(MO.getReg());
  if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());
  if (MO.isExpr())
    return 0; // The relocation has already been recorded at this point.
#ifndef NDEBUG
  errs() << MO;
#endif
  llvm_unreachable(0);
}

void TeeRISCMCCodeEmitter::EncodeInstruction(const MCInst &MI, raw_ostream &OS,
                                             SmallVectorImpl<MCFixup> &Fixups) const {
  unsigned Opcode = MI.getOpcode();
  MCNumEmitted++;

  OS << getBinaryCodeForInstr(MI);
}

// FIXME: These #defines shouldn't be necessary. Instead, tblgen should
// be able to generate code emitter helpers for either variant, like it
// does for the AsmWriter.
#define TeeRISCCodeEmitter TeeRISCMCCodeEmitter
#define MachineInstr MCInst
#include "TeeRISCGenCodeEmitter.inc"
#undef TeeRISCCodeEmitter
#undef MachineInstr
