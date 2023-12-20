cmake_minimum_required(VERSION 3.17)

set(APP vloop)

set(CMAKE_SYSTEM_NAME Generic)

if (CMAKE_SIZE STREQUAL "")
  message(FATAL_ERROR "You must provide CMAKE_SIZE option")
endif()

if (LIBRARIES_HOME STREQUAL "")
  message(FATAL_ERROR "LIBRARIES_HOME path needs to be defined.")
endif()

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

add_compile_options(-Wall -O2 -mtune=cortex-a53 -ffunction-sections -fdata-sections
    -fexceptions
    -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align
    -Wwrite-strings -Wmissing-declarations
    -Wredundant-decls -Wno-long-long
    -Wconversion)
add_compile_definitions(_DIOT=1)

add_link_options(
  -Wall
  -specs=nosys.specs
  -Wl,-gc-sections
  -Wl,-Map=output.map
  )

if (PERFORMANCE_TESTS)
  add_definitions(-DPERFORMANCE_TESTS)
endif()

project(${APP} C CXX ASM)

set(CMAKE_CXX_STANDARD 20)

### BSP

include(../bsp/bsp.cmake)

MakeBsp(1)

### END BSP

set(BMBOOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../bmboot)

include(${BMBOOT_DIR}/cmake/Bmboot.cmake)

# Include directories for headers
include_directories(
    ${CMAKE_SOURCE_DIR}/../utils # header files shared between fgc4 bare-metal applications
)

# VSLIB is treated as external library, as it would be in vloop applications
add_subdirectory(../vslib ${CMAKE_CURRENT_BINARY_DIR}/vslib)

add_bmboot_payload(${APP}
  main.cpp
  )

set(BIN_TARGETS)
set(ELF_TARGETS)

foreach (TARGET ${${APP}_TARGETS})

  target_link_libraries(${TARGET} PRIVATE
    vslib
    fmt::fmt
    bmboot_payload_runtime
    fgc4_bsp_el1
    )

  list(APPEND BIN_TARGETS "${CMAKE_BINARY_DIR}/${TARGET}.bin")
  list(APPEND ELF_TARGETS "${CMAKE_BINARY_DIR}/${TARGET}.elf")

endforeach()

############################################################
# Installation
############################################################

set(INSTALL_DIR ${CMAKE_BINARY_DIR}/bin)

install(FILES ${BIN_TARGETS} ${ELF_TARGETS}
        DESTINATION ${INSTALL_DIR})

############################################################
# Packaging
############################################################

set(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_BINARY_DIR})

# Sets creating deb package and tarball only
set(CPACK_BINARY_DEB "ON")
set(CPACK_BINARY_TGZ "ON")
set(CPACK_BINARY_TZ "OFF")
set(CPACK_BINARY_STGZ "OFF")
set(CPACK_BINARY_NSIS "OFF")

set(CPACK_SOURCE_ZIP "ON")
set(CPACK_SOURCE_TBZ2 "OFF")
set(CPACK_SOURCE_TZ "OFF")
set(CPACK_SOURCE_TXZ "OFF")
set(CPACK_SOURCE_7Z "OFF")

# Set all necessary fields for deb package creation,
set(CPACK_PACKAGE_CONTACT "dominik.arominski@cern.ch")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dominik Arominski")

# Ignore hidden (e.g. .git) and build directories for source packaging
set(CPACK_SOURCE_IGNORE_FILES
  build
  git
  [Cc][Mm]ake
)

include(CPack)

string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UPPER)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/build_flags.txt "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_UPPER}}")    # needed for benchmark framework
