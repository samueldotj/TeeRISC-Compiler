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
// Addressing Modes
//===----------------------------------------------------------------------===//
def ADDR:   ComplexPattern<iPTR, 2, "SelectADDR", [frameindex], []>;

// Address operands
def MEM:    Operand<iPTR> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops ptr_rc, i32imm);
}

//===----------------------------------------------------------------------===// 
// Instructions
//===----------------------------------------------------------------------===//
def ADD:    F1<1, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 + $rS2", 
                [(set i32:$rD, (add i32:$rS1, i32:$rS2))]>;

def OR:     F1<7, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), 
                "$rD = $rS1 | $rS2", 
                [(set i32:$rD, (or i32:$rS1, i32:$rS2))]>;

def ADD_IMM:F2<12, (outs IntRegs:$rD), (ins IntRegs:$rS1, i32imm:$imm16), 
                "$rD = $rS1 + $imm16", 
                [(set i32:$rD, (add i32:$rS1, (i32 imm16:$imm16)))]>;

def OR_IMM: F2<13, (outs IntRegs:$rD), (ins IntRegs:$rS1, i32imm:$rS2), 
                "$rD = $rS1 | $rS2", 
                [(set i32:$rD, (or i32:$rS1, (i32 imm16:$rS2)))]>;

def LD:     F3<16, (outs IntRegs:$rD), (ins MEM:$rS1), 
                "$rD = *(uint32_t *) $rS1", 
                [(set i32:$rD, (load ADDR:$rS1))]>;

def ST:     F3<17, (outs), (ins MEM:$rD, IntRegs:$rS1), 
                "*(uint32_t *) $rD = $rS1", 
                [(store i32:$rS1, ADDR:$rD)]>;

let isCall = 1, Uses = [LR, R1] in
def CALL:   F1<19, (outs), (ins IntRegs:$rD), 
                "$rD()", 
                [(TeeRISCCALL i32:$rD)]>;

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
