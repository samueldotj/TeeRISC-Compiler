//===-- TeeRISCInstPrinter.cpp - Convert TeeRISC MCInst to assembly syntax --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an TeeRISC MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "TeeRISCInstPrinter.h"
#include "TeeRISC.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

// Include the auto-generated portion of the assembly writer.
#include "TeeRISCGenAsmWriter.inc"

void TeeRISCInstPrinter::printInst(const MCInst *MI, raw_ostream &O, StringRef Annot) {
  printInstruction(MI, O);
  printAnnotation(O, Annot);
}

void TeeRISCInstPrinter::printOperand(const MCInst *MI, unsigned OpNo, 
                                      raw_ostream &O, const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    O << getRegisterName(Op.getReg());
  } else if (Op.isImm()) {
    O << (int32_t)Op.getImm();
  } else {
    assert(Op.isExpr() && "unknown operand kind in printOperand");
    O << *Op.getExpr();
  }
}

void TeeRISCInstPrinter::printUnsignedImm(const MCInst *MI, int OpNo, raw_ostream &O) {
  const MCOperand &MO = MI->getOperand(OpNo);
  if (MO.isImm())
    O << (uint32_t)MO.getImm();
  else
    printOperand(MI, OpNo, O, NULL);
}

void TeeRISCInstPrinter::printMemOperand(const MCInst *MI, int OpNo,
                                         raw_ostream &O, const char *Modifier) {
  O << "(";
  printOperand(MI, OpNo, O, NULL);
  O << " + ";
  printOperand(MI, OpNo + 1, O, NULL);
  O << ")";
}
