//===-- TeeRISCRegisterInfo.td - TeeRISC Register defs -----*- tablegen -*-===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//  Declarations that describe the TeeRISC register file 
//===----------------------------------------------------------------------===//

class TeeRISCReg<string n> : Register<n> {
  field bits<5> Num;
  let Namespace = "TeeRISC";
}

// Registers are identified with 5-bit ID numbers.
// Ri - 32-bit integer registers
class Ri<bits<5> num, string n> : TeeRISCReg<n> {
  let Num = num;
}

// Integer registers
foreach i = 0-31 in
    def R#i : Ri<i, "R"##i>, DwarfRegNum<[i]>;

// Register classes.
def IntRegs : RegisterClass<"TeeRISC",
                    /* regTypes   */ [i32], 
                    /* alignment  */ 32, 
                    /* memberList */ (sequence "R%u", 0, 31)>;

