//===-- TeeRISCELFObjectWriter.cpp - TeeRISC ELF Writer ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TeeRISCMCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
  class TeeRISCELFObjectWriter : public MCELFObjectTargetWriter {
  public:
    TeeRISCELFObjectWriter(uint8_t OSABI);

    virtual ~TeeRISCELFObjectWriter();
  protected:
    virtual unsigned GetRelocType(const MCValue &Target, const MCFixup &Fixup,
                                  bool IsPCRel, bool IsRelocWithSymbol,
                                  int64_t Addend) const;
  };
}

TeeRISCELFObjectWriter::TeeRISCELFObjectWriter(uint8_t OSABI)
  : MCELFObjectTargetWriter(/*Is64Bit*/ false, OSABI, ELF::EM_TEERISC,
                            /*HasRelocationAddend*/ false) {}

TeeRISCELFObjectWriter::~TeeRISCELFObjectWriter() {
}

unsigned TeeRISCELFObjectWriter::GetRelocType(const MCValue &Target,
                                              const MCFixup &Fixup,
                                              bool IsPCRel,
                                              bool IsRelocWithSymbol,
                                              int64_t Addend) const {
  // determine the type of the relocation
  unsigned Type;
  if (IsPCRel) {
    switch ((unsigned)Fixup.getKind()) {
    default:
      llvm_unreachable("Unimplemented");
    case FK_PCRel_2:
      Type = ELF::R_TEERISC_32_PCREL;
      break;
    }
  } else {
    switch ((unsigned)Fixup.getKind()) {
    default: llvm_unreachable("invalid fixup kind!");
    case FK_Data_4:
    case FK_Data_2:
      Type = ELF::R_TEERISC_32;
      break;
    }
  }
  return Type;
}

MCObjectWriter *llvm::createTeeRISCELFObjectWriter(raw_ostream &OS,
                                                   uint8_t OSABI) {
  MCELFObjectTargetWriter *MOTW = new TeeRISCELFObjectWriter(OSABI);
  return createELFObjectWriter(MOTW, OS,  /*IsLittleEndian=*/ true);
}
