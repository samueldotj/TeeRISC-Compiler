//= TeeRISCInstPrinter.h - Convert TeeRISC MCInst to assembly syntax -*- C++ -*-//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints a TeeRISC MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#ifndef TEERISCINSTPRINTER_H
#define TEERISCINSTPRINTER_H

#include "llvm/MC/MCInstPrinter.h"

namespace llvm {
  class MCOperand;

  class TeeRISCInstPrinter : public MCInstPrinter {
  public:
    TeeRISCInstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII, const MCRegisterInfo &MRI)
      : MCInstPrinter(MAI, MII, MRI) {}

    virtual void printInst(const MCInst *MI, raw_ostream &O, StringRef Annot);

    // Autogenerated by tblgen.
    void printInstruction(const MCInst *MI, raw_ostream &O);
    static const char *getRegisterName(unsigned RegNo);

    void printOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O, const char *Modifier = 0);
    void printUnsignedImm(const MCInst *MI, int OpNo, raw_ostream &O);
    void printMemOperand(const MCInst *MI, int OpNo,raw_ostream &O, const char *Modifier = 0);
  };
}

#endif