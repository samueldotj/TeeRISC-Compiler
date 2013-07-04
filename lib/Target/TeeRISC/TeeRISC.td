//===-- TeeRISC.td - Describe the TeeRISC Target Machine ----*- tablegen -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// TeeRISC Subtarget features.
//===----------------------------------------------------------------------===//

def FeatureV2
  : SubtargetFeature<"v2", "IsV2", "true", "Enable TeeRISC-V2 instructions">;
                    

//===----------------------------------------------------------------------===//
// Register File, Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "TeeRISCRegisterInfo.td"
include "TeeRISCCallingConv.td"
include "TeeRISCInstrInfo.td"

def TeeRISCInstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// TeeRISC Processors supported
//===----------------------------------------------------------------------===//
class Proc<string Name, list<SubtargetFeature> Features>
  : Processor<Name, NoItineraries, Features>;

def : Proc<"generic", []>;
def : Proc<"v1", []>;

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

def TeeRISC : Target {
  // Pull in Instruction Info:
  let InstructionSet = TeeRISCInstrInfo;
}
