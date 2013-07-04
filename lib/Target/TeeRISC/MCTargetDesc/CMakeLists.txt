add_llvm_library(LLVMTeeRISCDesc
  TeeRISCAsmBackend.cpp
  TeeRISCMCTargetDesc.cpp
  TeeRISCMCCodeEmitter.cpp
  TeeRISCMCAsmInfo.cpp
  TeeRISCELFObjectWriter.cpp
  )

add_dependencies(LLVMTeeRISCDesc TeeRISCCommonTableGen)
