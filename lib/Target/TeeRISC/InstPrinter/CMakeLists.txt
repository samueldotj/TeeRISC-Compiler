include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. 
                     ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_library(LLVMTeeRISCAsmPrinter
  TeeRISCInstPrinter.cpp
  )

add_dependencies(LLVMTeeRISCAsmPrinter TeeRISCCommonTableGen)
