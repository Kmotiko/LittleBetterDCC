set(CMAKE_VERBOSE_MAKEFILE 1)
cmake_minimum_required(VERSION 2.8)
PROJECT(DummyCCompiler CXX)

enable_testing()

find_package(Boost 1.55.0)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

if(NOT CMAKE_CXX_COMPILER)
  set(CMAKE_CXX_COMPILER clang++-3.6)
endif()

if(NOT LLVM_CONFIG)
  set(LLVM_CONFIG llvm-config-3.6)
endif()

execute_process(COMMAND ${LLVM_CONFIG}  --libdir
  OUTPUT_VARIABLE LLVM_LIB_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
execute_process(COMMAND ${LLVM_CONFIG}  --includedir
  OUTPUT_VARIABLE LLVM_INCLUDE_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
set (LLVM_CXX_FLAGS
  -D_DEBUG -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -O3 -fvisibility-inlines-hidden -fno-common -Wcast-qual
)
# Now, listing all of llvm libraries
# TODO: rewrite using CMake command to list lib names
set (LLVM_LIBS
     ncurses
     pthread
     z
     m
     dl
     LLVMLTO
     LLVMObjCARCOpts
     LLVMLinker
     LLVMBitWriter
     LLVMIRReader
     LLVMAsmParser
     LLVMR600CodeGen
     LLVMipo
     LLVMVectorize
     LLVMR600AsmParser
     LLVMR600Desc
     LLVMR600Info
     LLVMR600AsmPrinter
     LLVMSystemZDisassembler
     LLVMSystemZCodeGen
     LLVMSystemZAsmParser
     LLVMSystemZDesc
     LLVMSystemZInfo
     LLVMSystemZAsmPrinter
     LLVMHexagonDisassembler
     LLVMHexagonCodeGen
     LLVMHexagonDesc
     LLVMHexagonInfo
     LLVMNVPTXCodeGen
     LLVMNVPTXDesc
     LLVMNVPTXInfo
     LLVMNVPTXAsmPrinter
     LLVMCppBackendCodeGen
     LLVMCppBackendInfo
     LLVMMSP430CodeGen
     LLVMMSP430Desc
     LLVMMSP430Info
     LLVMMSP430AsmPrinter
     LLVMXCoreDisassembler
     LLVMXCoreCodeGen
     LLVMXCoreDesc
     LLVMXCoreInfo
     LLVMXCoreAsmPrinter
     LLVMMipsDisassembler
     LLVMMipsCodeGen
     LLVMMipsAsmParser
     LLVMMipsDesc
     LLVMMipsInfo
     LLVMMipsAsmPrinter
     LLVMAArch64Disassembler
     LLVMAArch64CodeGen
     LLVMAArch64AsmParser
     LLVMAArch64Desc
     LLVMAArch64Info
     LLVMAArch64AsmPrinter
     LLVMAArch64Utils
     LLVMARMDisassembler
     LLVMARMCodeGen
     LLVMARMAsmParser
     LLVMARMDesc
     LLVMARMInfo
     LLVMARMAsmPrinter
     LLVMPowerPCDisassembler
     LLVMPowerPCCodeGen
     LLVMPowerPCAsmParser
     LLVMPowerPCDesc
     LLVMPowerPCInfo
     LLVMPowerPCAsmPrinter
     LLVMSparcDisassembler
     LLVMSparcCodeGen
     LLVMSparcAsmParser
     LLVMSparcDesc
     LLVMSparcInfo
     LLVMSparcAsmPrinter
     LLVMTableGen
     LLVMDebugInfo
     LLVMOption
     LLVMX86Disassembler
     LLVMX86AsmParser
     LLVMX86CodeGen
     LLVMSelectionDAG
     LLVMAsmPrinter
     LLVMX86Desc
     LLVMMCDisassembler
     LLVMX86Info
     LLVMX86AsmPrinter
     LLVMX86Utils
     LLVMMCJIT
     LLVMLineEditor
     LLVMInstrumentation
     LLVMInterpreter
     LLVMExecutionEngine
     LLVMRuntimeDyld
     LLVMCodeGen
     LLVMScalarOpts
     LLVMProfileData
     LLVMObject
     LLVMMCParser
     LLVMBitReader
     LLVMInstCombine
     LLVMTransformUtils
     LLVMipa
     LLVMAnalysis
     LLVMTarget
     LLVMMC
     LLVMCore
     LLVMSupport
     )

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${LLVM_INCLUDE_DIR}")
link_directories("${LLVM_LIB_DIR}")

add_definitions(-std=c++1y -Wall)
add_definitions(${LLVM_CXX_FLAGS})

add_subdirectory(lib)
add_subdirectory(test)
