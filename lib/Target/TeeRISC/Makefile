##===- lib/Target/TeeRISC/Makefile -------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMTeeRISCCodeGen
TARGET = TeeRISC

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = TeeRISCGenRegisterInfo.inc TeeRISCGenInstrInfo.inc \
		TeeRISCGenAsmWriter.inc TeeRISCGenDAGISel.inc \
		TeeRISCGenSubtargetInfo.inc TeeRISCGenCallingConv.inc \
		TeeRISCSubtargetInfo.inc TeeRISCGenAsmWriter.inc

DIRS = InstPrinter TargetInfo MCTargetDesc

include $(LEVEL)/Makefile.common

