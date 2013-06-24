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
def simm16  : PatLeaf<(imm), [{ return isInt<16>(N->getSExtValue()); }]>;
def imm16   : PatLeaf<(imm), [{ return isUInt<16>(N->getSExtValue()); }]>;

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


def SDT_TeeRISCCall : SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;
def call            : SDNode<"TeeRISC_ISD::CALL", SDT_TeeRISCCall,
                            [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue,
                             SDNPVariadic]>;

def SDT_TeeRISCRet  : SDTypeProfile<0, 1, [SDTCisInt<0>]>;
def TeeRISCRET      : SDNode<"TeeRISC_ISD::RET", SDT_TeeRISCRet,
                            [SDNPHasChain, SDNPOptInGlue]>;
 
// Instructions
def ADD:    F1<1, (outs IntRegs:$dst), (ins IntRegs:$b, IntRegs:$c), 
                "$dst = $b + $c", 
                [(set i32:$dst, (add i32:$b, i32:$c))]>;

def OR:     F1<7, (outs IntRegs:$dst), (ins IntRegs:$b, IntRegs:$c), 
                "$dst = $b | $c", 
                [(set i32:$dst, (or i32:$b, i32:$c))]>;

def OR_IMM: F1<12, (outs IntRegs:$dst), (ins IntRegs:$b, i32imm:$c), 
                "$dst = $b", 
                [(set i32:$dst, (or i32:$b, (i32 imm16:$c)))]>;

def LD:     F3<16, (outs IntRegs:$dst), (ins IntRegs:$b), 
                "$dst = *(uint32_t *) $b", 
                [(set i32:$dst, (load i32:$b))]>;

def ST:     F3<17, (outs), (ins IntRegs:$dst, IntRegs:$b), 
                "*(uint32_t *)$b = $dst", 
                [(store i32:$b, i32:$dst)]>;

def CALL:   F1<19, (outs), (ins IntRegs:$dst), 
                "$dst()", 
                [(call i32:$dst)]>;

//Pseudo Instructions
let isReturn=1, isTerminator=1, hasDelaySlot=1, isCodeGenOnly=1,
    isBarrier=1, hasCtrlDep=1 in
  def RET:  Pseudo<(outs), (ins IntRegs:$dst),
                  "return $dst",
                  [(TeeRISCRET IntRegs:$dst)]>;

def SETHIi: Pseudo<(outs IntRegs:$dst), (ins i32imm:$src),
                   "sethi $src, $dst",
                   [(set i32:$dst, SETHIimm:$src)]>;

//===----------------------------------------------------------------------===//
// Non-Instruction Patterns
//===----------------------------------------------------------------------===//

// Small immediates.
def : Pat<(i32 imm16:$val),
          (OR_IMM (i32 R0), imm:$val)>;

// Arbitrary immediates.
def : Pat<(i32 imm:$val),
          (OR_IMM (SETHIi (HI16 imm:$val)), (LO16 imm:$val))>;

