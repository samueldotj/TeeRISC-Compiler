//===-- TeeRISCRegisterInfo.td - TeeRISC Register defs -----*- tablegen -*-===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//  Declarations that describe the TeeRISC register file 
//===----------------------------------------------------------------------===//

// Registers are identified with 5-bit ID numbers.
class TeeRISCReg<bits<5> num, string name, list<string> alt=[]> : Register<name, alt> {
  field bits<5> Num = num;
  let Namespace = "TeeRISC";
}

// Integer registers
foreach i = {0-31} in
  def R#i:  TeeRISCReg<i, "R"##i>, DwarfRegNum<[i]>;

// Register Aliases
def ZERO:   TeeRISCReg<0, "ZERO"> {
  let Aliases = [R0];
}
def FP:     TeeRISCReg<14, "FP"> {
  let Aliases = [R14];
}
def SP:     TeeRISCReg<15, "SP"> {
  let Aliases = [R15];
}
def IP:     TeeRISCReg<16, "IP"> {
  let Aliases = [R16];
}
def LR:     TeeRISCReg<17, "LR"> {
  let Aliases = [R17];
}
def I_LR:   TeeRISCReg<18, "I_LR"> {
  let Aliases = [R18];
}
def E_LR:   TeeRISCReg<19, "E_LR"> {
  let Aliases = [R19];
}

// Register classes.
def IntRegs : RegisterClass</* namespace       */ "TeeRISC", 
                            /* register types  */ [i32], 
                            /* alignment       */ 32, 
                            /* memberList      */ (sequence "R%u", 0, 31)>;

