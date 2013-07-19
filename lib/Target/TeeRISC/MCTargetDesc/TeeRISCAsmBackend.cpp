//===-- TeeRISCAsmBackend.cpp - TeeRISC Assembler Backend -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TeeRISCMCTargetDesc.h"
#include "llvm/ADT/Twine.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmLayout.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCELFSymbolFlags.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCSectionMachO.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ELF.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

static unsigned getFixupKindSize(unsigned Kind) {
  switch (Kind) {
  default: llvm_unreachable("invalid fixup kind!");
  case FK_Data_1: return 1;
  case FK_PCRel_2:
  case FK_Data_2: return 2;
  case FK_PCRel_4:
  case FK_Data_4: return 4;
  case FK_Data_8: return 8;
  }
}


namespace {

class TeeRISCAsmBackend : public MCAsmBackend {
public:
  TeeRISCAsmBackend(const Target &T): MCAsmBackend() {
  }

  unsigned getNumFixupKinds() const {
    return 2;
  }

  bool mayNeedRelaxation(const MCInst &Inst) const {
    return false;
  }

  /// fixupNeedsRelaxation - Target specific predicate for whether a given
  /// fixup requires the associated instruction to be relaxed.
  bool fixupNeedsRelaxation(const MCFixup &Fixup,
                            uint64_t Value,
                            const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const {
    assert(0 && "RelaxInstruction() is not implemented");
    return false;
  }

  void relaxInstruction(const MCInst &Inst, MCInst &Res) const {
      assert(0);
  }

  /// WriteNopData - Write an (optimal) nop sequence of Count bytes
  bool writeNopData(uint64_t Count, MCObjectWriter *OW) const {

/* TODO - remove the following
   Somehow function is not aligning to 4 bytes(Although all instructions are 4 bytes).
   Debugging why it is not aligned is difficult without Disassembler
   So add disassembler and then fix this issue and remove the following code.
 */
#if 1
    for (uint64_t i = 0; i < Count; i ++)
      OW->Write8(0);
    return true;
#endif

    if ((Count % 4) != 0)
      return false;

    for (uint64_t i = 0; i < Count; i += 4)
      OW->Write32(0x00000000);

    return true;
  }

  unsigned getPointerSize() const {
    return 4;
  }
};
} // end anonymous namespace

namespace {
class ELFTeeRISCAsmBackend : public TeeRISCAsmBackend {
public:
  uint8_t OSABI;
  ELFTeeRISCAsmBackend(const Target &T, uint8_t _OSABI)
    : TeeRISCAsmBackend(T), OSABI(_OSABI) { }

  void applyFixup(const MCFixup &Fixup, char *Data, unsigned DataSize,
                  uint64_t Value) const;

  MCObjectWriter *createObjectWriter(raw_ostream &OS) const {
    return createTeeRISCELFObjectWriter(OS, OSABI);
  }
};

void ELFTeeRISCAsmBackend::applyFixup(const MCFixup &Fixup, char *Data,
                                      unsigned DataSize, uint64_t Value) const {
  unsigned Size = getFixupKindSize(Fixup.getKind());

  assert(Fixup.getOffset() + Size <= DataSize &&
         "Invalid fixup offset!");

  char *data = Data + Fixup.getOffset();
  switch (Size) {
  default: llvm_unreachable("Cannot fixup unknown value.");
  case 1:  llvm_unreachable("Cannot fixup 1 byte value.");
  case 8:  llvm_unreachable("Cannot fixup 8 byte value.");

  case 4:
  case 2:
          llvm_unreachable("Write Code here to fixup\n");
  }
}
} // end anonymous namespace

MCAsmBackend *llvm::createTeeRISCAsmBackend(const Target &T, StringRef TT,
                                            StringRef CPU) {
  Triple TheTriple(TT);

  if (TheTriple.isOSDarwin())
    assert(0 && "Mac not supported on TeeRISC");

  if (TheTriple.isOSWindows())
    assert(0 && "Windows not supported on TeeRISC");

  uint8_t OSABI = MCELFObjectTargetWriter::getOSABI(TheTriple.getOS());
  return new ELFTeeRISCAsmBackend(T, OSABI);
}
