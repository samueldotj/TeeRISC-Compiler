#TeeRISC.td includes all other TeeRISC*.td files
set(LLVM_TARGET_DEFINITIONS TeeRISC.td)

tablegen(LLVM TeeRISCGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM TeeRISCGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM TeeRISCGenCodeEmitter.inc -gen-emitter)
tablegen(LLVM TeeRISCGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM TeeRISCGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM TeeRISCGenCallingConv.inc -gen-callingconv)
tablegen(LLVM TeeRISCGenSubtargetInfo.inc -gen-subtarget)

tablegen(LLVM TeeRISCGenDisassemblerTables.inc -gen-disassembler)

add_public_tablegen_target(TeeRISCCommonTableGen)

add_llvm_target(TeeRISCCodeGen
  TeeRISCAsmPrinter.cpp
  TeeRISCInstrInfo.cpp
  TeeRISCISelDAGToDAG.cpp
  TeeRISCISelLowering.cpp
  TeeRISCFrameLowering.cpp
  TeeRISCMachineFunctionInfo.cpp
  TeeRISCMCInstLower.cpp
  TeeRISCRegisterInfo.cpp
  TeeRISCTargetMachine.cpp
  TeeRISCSelectionDAGInfo.cpp
  TeeRISCSubtarget.cpp
  )

add_dependencies(LLVMTeeRISCCodeGen intrinsics_gen)

add_subdirectory(InstPrinter)
add_subdirectory(Disassembler)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
