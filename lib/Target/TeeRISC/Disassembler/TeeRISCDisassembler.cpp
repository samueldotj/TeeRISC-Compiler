//===- TeeRISCDisassembler.cpp - Disassembler for TeeRISC -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is part of the TeeRISC Disassembler.
//
//===----------------------------------------------------------------------===//

#include "TeeRISC.h"
#include "TeeRISCRegisterInfo.h"
#include "TeeRISCSubtarget.h"
#include "MCTargetDesc/TeeRISCBaseInfo.h"
#include "llvm/MC/MCDisassembler.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/MemoryObject.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {

/// TeeRISCDisassembler - a disasembler class for TeeRISC.
class TeeRISCDisassembler: public MCDisassembler {
public:
  /// Constructor     - Initializes the disassembler.
  TeeRISCDisassembler(const MCSubtargetInfo &STI):MCDisassembler(STI) {}

  /// getInstruction - See MCDisassembler.
  virtual DecodeStatus getInstruction(MCInst &instr,
                                      uint64_t &size,
                                      const MemoryObject &region,
                                      uint64_t address,
                                      raw_ostream &vStream,
                                      raw_ostream &cStream) const;
};

} // end anonymous namespace

// Forward declare these because the autogenerated code will reference them.
// Definitions are further down.
static DecodeStatus DecodeIntRegsRegisterClass(MCInst &Inst, unsigned RegNo,
                                               uint64_t Address, const void *Decoder);
static DecodeStatus DecodeMem(MCInst &Inst, unsigned Insn, uint64_t Address,
                              const void *Decoder);
namespace llvm {
  extern Target TheTeeRISCTarget;
}

static MCDisassembler *createTeeRISCDisassembler(const Target &T, const MCSubtargetInfo &STI) {
  return new TeeRISCDisassembler(STI);
}

extern "C" void LLVMInitializeTeeRISCDisassembler() {
  // Register the disassembler.
  TargetRegistry::RegisterMCDisassembler(TheTeeRISCTarget,  createTeeRISCDisassembler);
}

#include "TeeRISCGenDisassemblerTables.inc"
#include <stdio.h>

  /// readInstruction - read four bytes from the MemoryObject
  /// and return 32 bit word sorted according to the given endianess
static DecodeStatus readInstruction32(const MemoryObject &region, uint64_t address,
                                      uint64_t &size,  uint32_t &insn) {
  uint8_t Bytes[4];

  // We want to read exactly 4 Bytes of data.
  if (region.readBytes(address, 4, Bytes) == -1) {
    return MCDisassembler::Fail;
  }

  // Encoded as a small-endian 32-bit word in the stream.
  insn = (Bytes[0] <<  0) | (Bytes[1] <<  8) | (Bytes[2] << 16) | (Bytes[3] << 24);

  return MCDisassembler::Success;
}

DecodeStatus
TeeRISCDisassembler::getInstruction(MCInst &instr, uint64_t &Size,  const MemoryObject &Region,
                                    uint64_t Address, raw_ostream &vStream, raw_ostream &cStream) const {
  uint32_t Insn;

  DecodeStatus Result = readInstruction32(Region, Address, Size, Insn);
  if (Result == MCDisassembler::Fail)
    return MCDisassembler::Fail;

  // Calling the auto-generated decoder function.
  Result = decodeInstruction(DecoderTableTeeRISC32, instr, Insn, Address, this, STI);
  if (Result != MCDisassembler::Fail) {
    Size = 4;
    return Result;
  }

  return MCDisassembler::Fail;
}

static DecodeStatus DecodeIntRegsRegisterClass(MCInst &Inst, unsigned RegNo,
                                               uint64_t Address, const void *Decoder) {
  if (RegNo > 31)
    return MCDisassembler::Fail;

  RegNo = getTeeRISCRegisterFromNumbering(RegNo)  ;
  Inst.addOperand(MCOperand::CreateReg(RegNo));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeMem(MCInst &Inst, unsigned Insn,
                              uint64_t Address, const void *Decoder) {
  unsigned rD = fieldFromInstruction(Insn, 21, 5);
  unsigned rS1 = fieldFromInstruction(Insn, 16, 5);
  //int imm14 = SignExtend32<14>((Insn >> 2) & 0xffff);
  int imm14 = fieldFromInstruction(Insn, 0, 16);

  rD = getTeeRISCRegisterFromNumbering(rD);
  rS1 = getTeeRISCRegisterFromNumbering(rS1);

  Inst.addOperand(MCOperand::CreateReg(rD));
  
  Inst.addOperand(MCOperand::CreateImm(imm14));
  Inst.addOperand(MCOperand::CreateReg(rS1));

  return MCDisassembler::Success;
}