//===-- TeeRISCInstrFormats.td - TeeRISC Instruction Formats -*- tablegen -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Describe TeeRISC instructions format
//
//  CPU INSTRUCTION FORMATS
//
//  opcode  - operation code.
//  ra      - destination register
//  rb      - source register 1
//  rc      - source register 2
//  rd      - extra register
//  imm     - immediate
//  cond    - condion field
//
//===----------------------------------------------------------------------===//

// Format specifies the encoding used by the instruction.  This is part of the
// ad-hoc solution used to emit machine instruction encodings by our machine
// code emitter.
class Format<bits<4> val> {
  bits<4> Value = val;
}

def Form1 : Format<1>;
def Form2 : Format<2>;
def Form3 : Format<3>;
def FormP : Format<4>;

// Generic TeeRISC Format
class TeeRISCInst<dag outs, dag ins, string asmstr, list<dag> pattern, Format f>:
      Instruction
{
  field bits<32> Inst;
  Format Form = f;

  bits<6> Opcode = 0;
   
  // Attributes specific to TeeRISC instructions...
  bits<4> FormBits = Form.Value;

  let Namespace = "TeeRISC";
  let DecoderNamespace = "TeeRISC";

  let Size = 4;

  // Top 6 bits are the 'opcode' field
  let Inst{31-26} = Opcode;

  let OutOperandList = outs;
  let InOperandList  = ins;

  let AsmString   = asmstr;
  let Pattern     = pattern;

  // If the instruction is marked as a pseudo, set isCodeGenOnly so that the
  // assembler and disassmbler ignore it.
  let isCodeGenOnly = !eq(!cast<string>(Form), "Pseudo");

  // TableGen needs this field while generating Disassembler Tables.
  // TODO - figure out how to get rid of this field.
  field bits<32> SoftFail = 0;
}

//===----------------------------------------------------------------------===//
// Format 1 instruction class in TeeRISC : <|opcode|ra|rb|rc|rd|cond|>
//===----------------------------------------------------------------------===//

class F1<bits<6> op, bits<5> rExtra, bits<6> cond, 
         dag outs, dag ins, string asmstr, list<dag> pattern>:
      TeeRISCInst<outs, ins, asmstr, pattern, Form1>
{
  bits<5> rD;
  bits<5> rS1;
  bits<5> rS2;

  let Opcode = op;

  let Inst{25-21} = rD;
  let Inst{20-16} = rS1;
  let Inst{15-11} = rS2;
  let Inst{10-6}  = rExtra;
  let Inst{5-0}   = cond;
}

//===----------------------------------------------------------------------===//
// Format 2 instruction class in TeeRISC : <|opcode|ra|rb|imm|>
//===----------------------------------------------------------------------===//

class F2<bits<6> op, dag outs, dag ins, string asmstr, list<dag> pattern>: 
      TeeRISCInst<outs, ins, asmstr, pattern, Form2>
{
  bits<5> rD;
  bits<5> rS1;
  bits<16> imm16;

  let Opcode = op;

  let Inst{25-21} = rD;
  let Inst{20-16} = rS1;
  let Inst{15-0} = imm16;
}

//===----------------------------------------------------------------------===//
// Format 3 instruction class in TeeRISC : <|opcode|ra|rb|imm|bw|>
//===----------------------------------------------------------------------===//

class F3<bits<6> op, bits<2> bit_width, dag outs, dag ins, string asmstr, list<dag> pattern>:
      TeeRISCInst<outs, ins, asmstr, pattern, Form3>
{
  bits<5> rD;
  bits<19> addr;
   
  let Opcode = op;

  let Inst{25-21} = rD;
  let Inst{20-2} = addr;
  let Inst{1-0}  = bit_width;

  let DecoderMethod = "DecodeMem";
}

//===----------------------------------------------------------------------===//
// Pseudo instruction class.
//===----------------------------------------------------------------------===//
class Pseudo<dag outs, dag ins, string asmstr, list<dag> pattern>:
      TeeRISCInst<outs, ins, asmstr, pattern, FormP>;

