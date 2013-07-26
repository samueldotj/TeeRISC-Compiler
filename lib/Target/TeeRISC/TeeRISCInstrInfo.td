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


//===----------------------------------------------------------------------===//
// TeeRISC type profiles
//===----------------------------------------------------------------------===//
def SDT_TeeRISCCall         : SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;
def SDT_TeeRISCRet          : SDTypeProfile<0, 1, [SDTCisInt<0>]>;
def SDT_TeeRISCCallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>]>;
def SDT_TeeRISCCallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;

//===----------------------------------------------------------------------===//
// TeeRISC specific nodes
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
def ADD:    F1<0, ZERO.Num, CC_Always, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 + $rS2", 
                [(set IntRegs:$rD, (add IntRegs:$rS1, IntRegs:$rS2))]>;

def SUB:    F1<1, ZERO.Num, CC_Always, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 - $rS2", 
                [(set IntRegs:$rD, (sub IntRegs:$rS1, IntRegs:$rS2))]>;

//===----------------------------------------------------------------------===// 
// Logical Operations
//===----------------------------------------------------------------------===//
def AND:    F1<5, ZERO.Num, CC_Always, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 & $rS2", 
                [(set IntRegs:$rD, (and IntRegs:$rS1, IntRegs:$rS2))]>;

def OR:     F1<6, ZERO.Num, CC_Always, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 | $rS2", 
                [(set IntRegs:$rD, (or IntRegs:$rS1, IntRegs:$rS2))]>;

def XOR:    F1<7, ZERO.Num, CC_Always, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 ^ $rS2", 
                [(set IntRegs:$rD, (xor IntRegs:$rS1, IntRegs:$rS2))]>;

def LSHIFT: F1<8, ZERO.Num, CC_Always, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 << $rS2", 
                [(set IntRegs:$rD, (shl IntRegs:$rS1, IntRegs:$rS2))]>;

def RSHIFT: F1<9, ZERO.Num, CC_Always, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 >> $rS2", 
                [(set IntRegs:$rD, (srl IntRegs:$rS1, IntRegs:$rS2))]>;

//===----------------------------------------------------------------------===// 
// Operations on immediate
//===----------------------------------------------------------------------===//
def ADD_IMM:F2<10, (outs IntRegs:$rD), (ins IntRegs:$rS1, i32imm:$imm16), 
                "$rD = $rS1 + $imm16", 
                [(set IntRegs:$rD, (add IntRegs:$rS1, (i32 imm16:$imm16)))]>;

def AND_IMM:F2<11, (outs IntRegs:$rD), (ins IntRegs:$rS1, i32imm:$imm16), 
                "$rD = $rS1 & $imm16", 
                [(set IntRegs:$rD, (and IntRegs:$rS1, (i32 imm16:$imm16)))]>;

def OR_IMM: F2<12, (outs IntRegs:$rD), (ins IntRegs:$rS1, i32imm:$imm16), 
                "$rD = $rS1 | $imm16", 
                [(set IntRegs:$rD, (or IntRegs:$rS1, (i32 imm16:$imm16)))]>;
def LSHIFT_IMM: 
            F2<13, (outs IntRegs:$rD), (ins IntRegs:$rS1, i32imm:$imm16), 
                "$rD = $rS1 << $imm16", 
                [(set IntRegs:$rD, (shl IntRegs:$rS1, (i32 imm16:$imm16)))]>;

def RSHIFT_IMM: 
            F2<14, (outs IntRegs:$rD), (ins IntRegs:$rS1, i32imm:$imm16), 
                "$rD = $rS1 << $imm16", 
                [(set IntRegs:$rD, (srl IntRegs:$rS1, (i32 imm16:$imm16)))]>;
//===----------------------------------------------------------------------===// 
// Load/Store instructions
//===----------------------------------------------------------------------===//
let mayLoad = 1 in
def LD:     F3<20, AW_32Bit, (outs IntRegs:$rD), (ins memri:$addr), 
                "$rD = *(uint32_t *) $addr", 
                [(set IntRegs:$rD, (load iaddr:$addr))]>;

let mayStore = 1 in
def ST:     F3<21, AW_32Bit, (outs), (ins IntRegs:$rD, memri:$addr),
                "*(uint32_t *) $addr = $rD", 
               [(store IntRegs:$rD, iaddr:$addr)]>;

//===----------------------------------------------------------------------===// 
// Brranch instructions
//===----------------------------------------------------------------------===//
let isCall = 1, isBranch = 1, Uses = [LR],
    rS1 = ZERO.Num, rS2 = ZERO.Num in
def CALL:   F1<22, ZERO.Num, CC_Always, (outs), (ins IntRegs:$rD), 
                "$rD()", 
                [(TeeRISCCALL IntRegs:$rD)]>;

let isReturn=1, isTerminator=1,  isBarrier=1, hasCtrlDep=1, isCodeGenOnly=1,
    rD = IP.Num, rS1 = LR.Num, rS2 = ZERO.Num in
def RET:    F1<ADD.Opcode, ZERO.Num, CC_Always, (outs), (ins IntRegs:$dst),
                  "return $dst",
                  [(TeeRISCRET IntRegs:$dst)]>;

//===----------------------------------------------------------------------===//
// Pseudo Instructions
//===----------------------------------------------------------------------===//
// As stack alignment is always done with ADD, we need a 16-bit immediate
let isCodeGenOnly=1, isPseudo=1,
    Defs = [SP], Uses = [SP] in {
def ADJCALLSTACKDOWN:
            Pseudo<(outs), (ins i32imm:$amt),
                  "SP -= $amt",
                  [(callseq_start timm:$amt)]>;
def ADJCALLSTACKUP:
            Pseudo<(outs),
                  (ins i32imm:$amt1, i32imm:$amt2),
                  "SP += $amt1",
                  [(callseq_end timm:$amt1, timm:$amt2)]>;
}

//===----------------------------------------------------------------------===//
// Non-Instruction Patterns
//===----------------------------------------------------------------------===//

// Small immediates.
def : Pat<(i32 imm16:$val),
          (OR_IMM (i32 R0), imm:$val)>;

// Arbitrary 32bit immediate.
// rX = r0 | High_16bit
// rX = rX << 16
// rX = rx | Low_16bit
def : Pat<(i32 imm:$val),
          (OR_IMM 
            (LSHIFT_IMM 
              (OR_IMM (i32 R0), (HI16 imm:$val)), 
              (i32 16)),
            (LO16 imm:$val))>;
