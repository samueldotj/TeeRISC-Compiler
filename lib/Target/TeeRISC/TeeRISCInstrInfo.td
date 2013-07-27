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
    return CurDAG->getTargetConstant((unsigned)N->getZExtValue() & 0xffff, MVT::i32);
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

class ArithLogic<bits<6> op, SDNode OpNode, string asmstr, bit isComm = 0, bit isSigned=0>:
      F1<op, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), asmstr,
         [(set IntRegs:$rD, (OpNode IntRegs:$rS1, IntRegs:$rS2))]> {
    let isCommutable = isComm;
    let isSignedOperation = isSigned;
}

//===----------------------------------------------------------------------===// 
// Arithmetic Instructions
//===----------------------------------------------------------------------===//
def ADD:        ArithLogic<0, add,  "$rD = $rS1 + $rS2", 1>;
def SUB:        ArithLogic<1, sub,  "$rD = $rS1 - $rS2">;
def MUL:        ArithLogic<2, mul,  "$rD = $rS1 * $rS2", 1>;
def UDIV:       ArithLogic<3, udiv, "$rD = $rS1 / $rS2">;
def UMOD:       ArithLogic<4, sub,  "$rD = $rS1 % $rS2">;
def SDIV:       ArithLogic<3, sub,  "$rD = $rS1 / $rS2", 0, 1>;
def SMOD:       ArithLogic<4, sub,  "$rD = $rS1 % $rS2", 0, 1>;

//===----------------------------------------------------------------------===// 
// Logical Operations
//===----------------------------------------------------------------------===//
def AND:        ArithLogic<5, and,  "$rD = $rS1 & $rS2", 1>;
def OR:         ArithLogic<6, or,   "$rD = $rS1 | $rS2", 1>;
def XOR:        ArithLogic<7, xor,  "$rD = $rS1 ^ $rS2", 1>;
def LSHIFT:     ArithLogic<8, shl,  "$rD = $rS1 << $rS2">;
def RSHIFT:     ArithLogic<9, srl,  "$rD = $rS1 >> $rS2">;

//===----------------------------------------------------------------------===// 
// Operations on immediate
//===----------------------------------------------------------------------===//
class ArithLogicImm<bits<6> op, SDNode OpNode, string asmstr, bit isComm = 0>:
      F2<op, (outs IntRegs:$rD), (ins IntRegs:$rS1, i32imm:$imm16), asmstr,
         [(set IntRegs:$rD, (OpNode IntRegs:$rS1, (i32 imm16:$imm16)))]>;

def ADD_IMM:    ArithLogicImm<10, add,  "$rD = $rS1 + $imm16">;
def AND_IMM:    ArithLogicImm<11, and,  "$rD = $rS1 & $imm16">;
def OR_IMM:     ArithLogicImm<12, or,   "$rD = $rS1 | $imm16">;
def LSHIFT_IMM: ArithLogicImm<13, shl,  "$rD = $rS1 << $imm16">;
def RSHIFT_IMM: ArithLogicImm<14, shl,  "$rD = $rS1 >> $imm16">;

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
def CALL:   F1<22, (outs), (ins IntRegs:$rD), 
                "$rD()", 
                [(TeeRISCCALL IntRegs:$rD)]>;

let isReturn=1, isTerminator=1,  isBarrier=1, hasCtrlDep=1, isCodeGenOnly=1,
    rD = IP.Num, rS1 = LR.Num, rS2 = ZERO.Num in
def RET:    F1<ADD.Opcode, (outs), (ins IntRegs:$dst),
                  "return $dst",
                  [(TeeRISCRET IntRegs:$dst)]>;

//===----------------------------------------------------------------------===//
// Pseudo Instructions
//===----------------------------------------------------------------------===//
// As stack alignment is always done with ADD, we need a 16-bit immediate
let isCodeGenOnly=1, isPseudo=1, Defs = [SP], Uses = [SP] in {
def ADJCALLSTACKDOWN: Pseudo<(outs), (ins i32imm:$amt), 
                            "SP -= $amt", [(callseq_start timm:$amt)]>;

def ADJCALLSTACKUP:   Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2), 
                            "SP += $amt1", [(callseq_end timm:$amt1, timm:$amt2)]>;
}

//===----------------------------------------------------------------------===//
// Non-Instruction Patterns
//===----------------------------------------------------------------------===//

// Small immediates.
// rX = (R0 | Imm_16bit)
def : Pat<(i32 imm16:$val),
          (OR_IMM (i32 R0), imm:$val)>;

// Arbitrary 32bit immediate.
// rX = (((R0 | High_16bit) << 16) | Low_16bit)
// This will result in 3 instructions
def : Pat<(i32 imm:$val),
          (OR_IMM 
            (LSHIFT_IMM 
              (OR_IMM (i32 R0), (HI16 imm:$val)), (i32 16)),
            (LO16 imm:$val))>;
