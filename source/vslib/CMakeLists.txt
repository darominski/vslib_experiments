cmake_minimum_required(VERSION 3.17)

set(LIB vslib)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

add_compile_options(-Wall -O0 -mtune=cortex-a53 -ffunction-sections -fdata-sections
    -fexceptions
    -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align
    -Wwrite-strings -Wmissing-declarations
    -Wredundant-decls -Winline -Wno-long-long
    -Wconversion)

add_link_options(
  -Wall
  -specs=nosys.specs
  -Wl,-gc-sections
  -Wl,-Map=output.map
  )

project(${LIB} CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(../lib/fmt ${CMAKE_CURRENT_BINARY_DIR}/fmt)

# Collect source files for the vslib code
set(VSLIB_SOURCES
    background/src/background.cpp
    components/src/componentRegistry.cpp
    parameters/src/parameterRegistry.cpp
    # Other vslib source files go here
)

# Create a library from the vslib sources
add_library(${LIB} ${VSLIB_SOURCES})

# Include directories for vslib
target_include_directories(${LIB}
    PUBLIC
    background/inc
    components/inc
    parameters/inc
    ../utils
    ../lib
    # Other vslib include directories go here
)

target_link_libraries(${LIB} PRIVATE
  fmt::fmt
  )

############################################################
# Installation
############################################################

set(INSTALL_LIB_DIR ${CMAKE_BINARY_DIR}/lib)

# Specify the installation path for the library
install(TARGETS vslib
    ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
)

############################################################
# Tests
############################################################

if(BUILD_TESTS)

  if(NOT DEFINED CMAKE_CXX_COMPILER_NONE_LINUX_GNU)
  message(FATAL_ERROR "CMAKE_CXX_COMPILER_NONE_LINUX_GNU must be set")
  endif()

  enable_testing()

  include(ExternalProject)

  ExternalProject_Add(utilsTests
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../utils
    CMAKE_ARGS
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER_NONE_LINUX_GNU}
        -DLIBRARIES_HOME=${LIBRARIES_HOME}
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
    INSTALL_COMMAND ""
    BUILD_ALWAYS ON
  )

endif()
