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
// Arithmetic and Logical Instructions
//===----------------------------------------------------------------------===//
class ArithLogic<bits<6> op, SDNode OpNode, string asmstr, bit isComm = 0, bit isSigned=0>:
      F1<op, (outs IntRegs:$rD), (ins IntRegs:$rS1, IntRegs:$rS2), asmstr,
         [(set IntRegs:$rD, (OpNode IntRegs:$rS1, IntRegs:$rS2))]> {
    let isCommutable = isComm;
    let isSignedOperation = isSigned;
}

def ADD:        ArithLogic<0, add,  "$rD = $rS1 + $rS2", 1>;
def SUB:        ArithLogic<1, sub,  "$rD = $rS1 - $rS2">;
def MUL:        ArithLogic<2, mul,  "$rD = $rS1 * $rS2", 1>;
def UDIV:       ArithLogic<3, udiv, "$rD = $rS1 / $rS2">;
def UMOD:       ArithLogic<4, sub,  "$rD = $rS1 % $rS2">;
def SDIV:       ArithLogic<3, sub,  "$rD = $rS1 / $rS2", 0, 1>;
def SMOD:       ArithLogic<4, sub,  "$rD = $rS1 % $rS2", 0, 1>;

def AND:        ArithLogic<5, and,  "$rD = $rS1 & $rS2", 1>;
def OR:         ArithLogic<6, or,   "$rD = $rS1 | $rS2", 1>;
def XOR:        ArithLogic<7, xor,  "$rD = $rS1 ^ $rS2", 1>;
def LSHIFT:     ArithLogic<8, shl,  "$rD = $rS1 << $rS2">;
def RSHIFT:     ArithLogic<9, srl,  "$rD = $rS1 >> $rS2">;

//===----------------------------------------------------------------------===// 
// Operations on immediate
//===----------------------------------------------------------------------===//
// Immediate Pattern
def imm16   : PatLeaf<(imm), [{ return isUInt<16>(N->getSExtValue()); }]>;
class ArithLogicImm<bits<6> op, SDNode OpNode, string asmstr, bit isComm = 0>:
      F2<op, (outs IntRegs:$rD), (ins IntRegs:$rS1, i32imm:$imm16), asmstr,
         [(set IntRegs:$rD, (OpNode IntRegs:$rS1, (i32 imm16:$imm16)))]>;

def ADD_IMM:    ArithLogicImm<10, add,  "$rD = $rS1 + $imm16">;
def AND_IMM:    ArithLogicImm<11, and,  "$rD = $rS1 & $imm16">;
def OR_IMM:     ArithLogicImm<12, or,   "$rD = $rS1 | $imm16">;
def LSHIFT_IMM: ArithLogicImm<13, shl,  "$rD = $rS1 << $imm16">;
def RSHIFT_IMM: ArithLogicImm<14, shl,  "$rD = $rS1 >> $imm16">;

//===----------------------------------------------------------------------===//
// Memory Access Instructions
// TeeRISC supports only one addressing mode = [R+I]
//===----------------------------------------------------------------------===//
// Address Pattern
def iaddr : ComplexPattern<i32, 2, "SelectAddrRegImm", [frameindex], []>;

// Address operands
def memri:  Operand<i32> {
  let PrintMethod = "printMemOperand";
  let EncoderMethod = "getMemEncoding";
  
  let MIOperandInfo = (ops IntRegs, i32imm);
}

//===----------------------------------------------------------------------===// 
// Load instructions
//===----------------------------------------------------------------------===//
class LoadRI<PatFrag OpNode, AccessWidth aw, string asmstr> :
             F3<20, aw, (outs IntRegs:$rD), (ins memri:$addr), asmstr,
                [(set IntRegs:$rD, (OpNode iaddr:$addr))]> {
    let mayLoad = 1;
}

def LD:     LoadRI<load,            AW_32Bit, "$rD = *(uint32_t *) $addr">;
def LDH:    LoadRI<zextloadi16,     AW_16Bit, "$rD = *(uint16_t *) $addr">;
def LDB:    LoadRI<zextloadi8,      AW_8Bit,  "$rD = *(uint8_t *) $addr">;

//===----------------------------------------------------------------------===// 
// Store instructions
//===----------------------------------------------------------------------===//
class StoreRI<PatFrag OpNode, AccessWidth aw, string asmstr> :
             F3<21, aw, (outs), (ins IntRegs:$rD, memri:$addr), asmstr,
                [(OpNode IntRegs:$rD, iaddr:$addr)]> {
    let mayStore = 1;
}

def ST:     StoreRI<store,          AW_32Bit, "*(uint32_t *) $addr = $rD">;
def STH:    StoreRI<truncstorei16,  AW_16Bit, "*(uint16_t *) $addr = $rD">;
def STB:    StoreRI<truncstorei8,   AW_8Bit,  "*(uint8_t *) $addr = $rD">;

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
def LO16 : SDNodeXForm<imm, [{
    return CurDAG->getTargetConstant((unsigned)N->getZExtValue() & 0xffff, MVT::i32);
}]>;

def HI16 : SDNodeXForm<imm, [{
    // Transformation function: shift the immediate value down into the low bits.
    return CurDAG->getTargetConstant((unsigned)N->getZExtValue() >> 16, MVT::i32);
}]>;

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
