//===-- TeeRISCCallingConv.td - Calling Conventions TeeRISC -*- tablegen -*-===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the TeeRISC architectures.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// TeeRISC 32-bit.
//===----------------------------------------------------------------------===//

// R2-R6 is used for first 5 arguments
// Remaining arguments are passed through stack
def CC_Call_TeeRISC : CallingConv<[
    CCAssignToReg<[R2, R3, R4, R5, R6]>,
    CCAssignToStack<4, 4>
]>;

// R7-R11 are saved by the callee
def CC_CalleeSave_TeeRISC : CalleeSavedRegs<(add R7, R8, R9, R10, R11)>;

// R1 is used for return value
def CC_Ret_TeeRISC : CallingConv<[
    CCAssignToReg<[R1]>
]>;

