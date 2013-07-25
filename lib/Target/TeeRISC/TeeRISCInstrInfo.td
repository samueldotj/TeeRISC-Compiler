//===-- TeeRISCInstrInfo.td - Target Description for TeeRISC Target -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the TeeRISC instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//

include "TeeRISCInstrFormats.td"

//===----------------------------------------------------------------------===//
// Instruction Pattern Stuff
//===----------------------------------------------------------------------===//
def LO16 : SDNodeXForm<imm, [{
    return CurDAG->getTargetConstant((unsigned)N->getZExtValue() & 0xffff,
                                     MVT::i32);
}]>;

def HI16 : SDNodeXForm<imm, [{
    // Transformation function: shift the immediate value down into the low bits.
    return CurDAG->getTargetConstant((unsigned)N->getZExtValue() >> 16, MVT::i32);
}]>;

def SETHIimm : PatLeaf<(imm), [{
  return isShiftedUInt<16, 16>(N->getZExtValue());
}], HI16>;

//===----------------------------------------------------------------------===//
// MBlaze type profiles
//===----------------------------------------------------------------------===//
def SDT_TeeRISCCall         : SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;
def SDT_TeeRISCRet          : SDTypeProfile<0, 1, [SDTCisInt<0>]>;
def SDT_TeeRISCCallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>]>;
def SDT_TeeRISCCallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;

//===----------------------------------------------------------------------===//
// MBlaze specific nodes
//===----------------------------------------------------------------------===//
def TeeRISCCALL   : SDNode<"TeeRISC_ISD::CALL", SDT_TeeRISCCall,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue, SDNPVariadic]>;
def TeeRISCRET    : SDNode<"TeeRISC_ISD::RET", SDT_TeeRISCRet,
                           [SDNPHasChain, SDNPOptInGlue]>;


def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_TeeRISCCallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;

def callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_TeeRISCCallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;


//===----------------------------------------------------------------------===//
// Operand Types
//===----------------------------------------------------------------------===//
def imm16   : PatLeaf<(imm), [{ return isUInt<16>(N->getSExtValue()); }]>;

//===----------------------------------------------------------------------===//
// Addressing Modes
//===----------------------------------------------------------------------===//
def iaddr : ComplexPattern<i32, 2, "SelectAddrRegImm", [frameindex], []>;

// Address operands
def memri:  Operand<i32> {
  let PrintMethod = "printMemOperand";
  let EncoderMethod = "getMemEncoding";
  
  let MIOperandInfo = (ops IntRegs, i32imm);
}

//===----------------------------------------------------------------------===// 
// Instructions
//===----------------------------------------------------------------------===//
def ADD:    F1<1, 0, CC_Always, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 + $rS2", 
                [(set IntRegs:$rD, (add IntRegs:$rS1, IntRegs:$rS2))]>;

def SUB:    F1<2, 0, CC_Always, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 - $rS2", 
                [(set IntRegs:$rD, (sub IntRegs:$rS1, IntRegs:$rS2))]>;

//===----------------------------------------------------------------------===// 
// Logical Operations
//===----------------------------------------------------------------------===//
def AND:    F1<6, 0, CC_Always, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 & $rS2", 
                [(set IntRegs:$rD, (and IntRegs:$rS1, IntRegs:$rS2))]>;

def OR:     F1<7, 0, CC_Always, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 | $rS2", 
                [(set IntRegs:$rD, (or IntRegs:$rS1, IntRegs:$rS2))]>;

def XOR:    F1<8, 0, CC_Always, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 ^ $rS2", 
                [(set IntRegs:$rD, (xor IntRegs:$rS1, IntRegs:$rS2))]>;

def LSHIFT: F1<10, 0, CC_Always, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 << $rS2", 
                [(set IntRegs:$rD, (shl IntRegs:$rS1, IntRegs:$rS2))]>;

def RSHIFT: F1<11, 0, CC_Always, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 >> $rS2", 
                [(set IntRegs:$rD, (srl IntRegs:$rS1, IntRegs:$rS2))]>;

//===----------------------------------------------------------------------===// 
// Operations on immediate
//===----------------------------------------------------------------------===//
def ADD_IMM:F2<12, (outs IntRegs:$rD), (ins IntRegs:$rS1, i32imm:$imm16), 
                "$rD = $rS1 + $imm16", 
                [(set IntRegs:$rD, (add IntRegs:$rS1, (i32 imm16:$imm16)))]>;

def OR_IMM: F2<13, (outs IntRegs:$rD), (ins IntRegs:$rS1, i32imm:$imm16), 
                "$rD = $rS1 | $imm16", 
                [(set IntRegs:$rD, (or IntRegs:$rS1, (i32 imm16:$imm16)))]>;

//===----------------------------------------------------------------------===// 
// Load/Store instructions
//===----------------------------------------------------------------------===//
let mayLoad = 1 in
def LD:     F3<16, AW_32Bit, (outs IntRegs:$rD), (ins memri:$addr), 
                "$rD = *(uint32_t *) $addr", 
                [(set IntRegs:$rD, (load iaddr:$addr))]>;

let mayStore = 1 in
def ST:     F3<17, AW_32Bit, (outs), (ins IntRegs:$rD, memri:$addr),
                "*(uint32_t *) $addr = $rD", 
               [(store IntRegs:$rD, iaddr:$addr)]>;

//===----------------------------------------------------------------------===// 
// Brranch instructions
//===----------------------------------------------------------------------===//
let isCall = 1, isBranch = 1, Uses = [LR, R1],
    rS1 = 0, rS2 = 0 in
def CALL:   F1<19, 0, CC_Always, (outs), (ins IntRegs:$rD), 
                "$rD()", 
                [(TeeRISCCALL IntRegs:$rD)]>;

//===----------------------------------------------------------------------===//
// Pseudo Instructions
//===----------------------------------------------------------------------===//
let isReturn=1, isTerminator=1, isCodeGenOnly=1,
    isBarrier=1, hasCtrlDep=1 in
def RET:    Pseudo<(outs), (ins IntRegs:$dst),
                  "return $dst",
                  [(TeeRISCRET IntRegs:$dst)]>;

def SETHIi: Pseudo<(outs IntRegs:$dst), (ins i32imm:$src),
                   "sethi $src, $dst",
                   [(set i32:$dst, SETHIimm:$src)]>;

// As stack alignment is always done with addiu, we need a 16-bit immediate
let Defs = [SP], Uses = [SP] in {
def ADJCALLSTACKDOWN:
            Pseudo<(outs), (ins i32imm:$amt),
                  "#ADJCALLSTACKDOWN $amt",
                  [(callseq_start timm:$amt)]>;
def ADJCALLSTACKUP:
            Pseudo<(outs),
                  (ins i32imm:$amt1, i32imm:$amt2),
                  "#ADJCALLSTACKUP $amt1",
                  [(callseq_end timm:$amt1, timm:$amt2)]>;
}

//===----------------------------------------------------------------------===//
// Non-Instruction Patterns
//===----------------------------------------------------------------------===//

// Small immediates.
def : Pat<(i32 imm16:$val),
          (OR_IMM (i32 R0), imm:$val)>;

// Arbitrary immediates.
def : Pat<(i32 imm:$val),
          (OR_IMM (SETHIi (HI16 imm:$val)), (LO16 imm:$val))>;
