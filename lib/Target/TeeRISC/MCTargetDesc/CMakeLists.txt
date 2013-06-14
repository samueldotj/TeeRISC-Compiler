add_llvm_library(LLVMTeeRISCDesc
  TeeRISCMCTargetDesc.cpp
  TeeRISCMCAsmInfo.cpp
  )

add_dependencies(LLVMTeeRISCDesc TeeRISCCommonTableGen)
