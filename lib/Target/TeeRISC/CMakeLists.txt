#TeeRISC.td includes all other TeeRISC*.td files
set(LLVM_TARGET_DEFINITIONS TeeRISC.td)

tablegen(LLVM TeeRISCGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM TeeRISCGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM TeeRISCGenAsmWriter.inc -gen-asm-writer)
#tablegen(LLVM TeeRISCGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM TeeRISCGenCallingConv.inc -gen-callingconv)

add_public_tablegen_target(TeeRISCCommonTableGen)

add_llvm_target(TeeRISCCodeGen
  TeeRISCAsmPrinter.cpp
#  TeeRISCInstrInfo.cpp
#  TeeRISCISelDAGToDAG.cpp
#  TeeRISCISelLowering.cpp
#  TeeRISCFrameLowering.cpp
#  TeeRISCMachineFunctionInfo.cpp
#  TeeRISCRegisterInfo.cpp
#  TeeRISCSubtarget.cpp
  TeeRISCTargetMachine.cpp
#  TeeRISCSelectionDAGInfo.cpp
  )

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
