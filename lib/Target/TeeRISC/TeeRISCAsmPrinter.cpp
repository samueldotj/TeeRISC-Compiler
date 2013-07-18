//===-- TeeRISCAsmPrinter.cpp - TeeRISC LLVM assembly writer ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format TeeRISC assembly language.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "teerisc-asm-printer"

#include "TeeRISC.h"
#include "InstPrinter/TeeRISCInstPrinter.h"
#include "MCTargetDesc/TeeRISCBaseInfo.h"
#include "TeeRISCInstrInfo.h"
#include "TeeRISCMCInstLower.h"
#include "TeeRISCSubtarget.h"
#include "TeeRISCTargetMachine.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/Mangler.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include <cctype>

using namespace llvm;

namespace {
  class TeeRISCAsmPrinter : public AsmPrinter {
    const TeeRISCSubtarget *Subtarget;
  public:
    explicit TeeRISCAsmPrinter(TargetMachine &TM, MCStreamer &Streamer)
      : AsmPrinter(TM, Streamer) {
      Subtarget = &TM.getSubtarget<TeeRISCSubtarget>();
    }

    virtual const char *getPassName() const {
      return "TeeRISC Assembly Printer";
    }

    void emitFrameDirective();
    virtual void EmitFunctionBodyStart();
    virtual void EmitFunctionBodyEnd();
    virtual void EmitFunctionEntryLabel();

    void printOperand(const MachineInstr *MI, int opNum, raw_ostream &O);
    void EmitInstruction(const MachineInstr *MI);
  };
} // end of anonymous namespace

// #include "TeeRISCGenAsmWriter.inc"

/// Frame Directive
void TeeRISCAsmPrinter::emitFrameDirective() {
  if (!OutStreamer.hasRawTextSupport())
    return;

  const TargetRegisterInfo &RI = *TM.getRegisterInfo();
  unsigned stkReg = RI.getFrameRegister(*MF);
  unsigned retReg = RI.getRARegister();
  unsigned stkSze = MF->getFrameInfo()->getStackSize();

  OutStreamer.EmitRawText("\t.frame\t" +
                          Twine(TeeRISCInstPrinter::getRegisterName(stkReg)) +
                          ", " + Twine(stkSze) + ", " +
                          Twine(TeeRISCInstPrinter::getRegisterName(retReg)));
}

void TeeRISCAsmPrinter::EmitFunctionEntryLabel() {
  if (OutStreamer.hasRawTextSupport())
    OutStreamer.EmitRawText("\t.entry\t" + Twine(CurrentFnSym->getName()));
  AsmPrinter::EmitFunctionEntryLabel();
}

void TeeRISCAsmPrinter::EmitFunctionBodyStart() {
  if (!OutStreamer.hasRawTextSupport())
    return;

  emitFrameDirective();
}

void TeeRISCAsmPrinter::EmitFunctionBodyEnd() {
  if (OutStreamer.hasRawTextSupport())
    OutStreamer.EmitRawText("\t.end\t" + Twine(CurrentFnSym->getName()));
}

//===----------------------------------------------------------------------===//
void TeeRISCAsmPrinter::EmitInstruction(const MachineInstr *MI) {
  TeeRISCMCInstLower MCInstLowering(OutContext, *this);

  MCInst TmpInst;
  MCInstLowering.Lower(MI, TmpInst);
  OutStreamer.EmitInstruction(TmpInst);
}

void TeeRISCAsmPrinter::printOperand(const MachineInstr *MI, int opNum,
                                     raw_ostream &O) {
  const MachineOperand &MO = MI->getOperand(opNum);

  switch (MO.getType()) {
  case MachineOperand::MO_Register:
    O << TeeRISCInstPrinter::getRegisterName(MO.getReg());
    break;

  case MachineOperand::MO_Immediate:
    O << (int32_t)MO.getImm();
    break;

  case MachineOperand::MO_MachineBasicBlock:
    O << *MO.getMBB()->getSymbol();
    return;

  case MachineOperand::MO_GlobalAddress:
    O << *Mang->getSymbol(MO.getGlobal());
    break;

  case MachineOperand::MO_ExternalSymbol:
    O << *GetExternalSymbolSymbol(MO.getSymbolName());
    break;

  case MachineOperand::MO_JumpTableIndex:
    O << MAI->getPrivateGlobalPrefix() << "JTI" << getFunctionNumber()
      << '_' << MO.getIndex();
    break;

  case MachineOperand::MO_ConstantPoolIndex:
    O << MAI->getPrivateGlobalPrefix() << "CPI"
      << getFunctionNumber() << "_" << MO.getIndex();
    if (MO.getOffset())
      O << "+" << MO.getOffset();
    break;

  default:
    llvm_unreachable("<unknown operand type>");
  }
}

// Force static initialization.
extern "C" void LLVMInitializeTeeRISCAsmPrinter() {
  RegisterAsmPrinter<TeeRISCAsmPrinter> X(TheTeeRISCTarget);
}
