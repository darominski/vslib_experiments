cmake_minimum_required(VERSION 3.17)

set(APP vloop)

set(CMAKE_SYSTEM_NAME Generic)

if (CMAKE_SIZE STREQUAL "")
  message(FATAL_ERROR "You must provide CMAKE_SIZE option")
endif()

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

add_compile_options(-Wall -O0 -mtune=cortex-a53 -ffunction-sections -fdata-sections
    -fexceptions
    -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align
    -Wwrite-strings -Wmissing-declarations
    -Wredundant-decls -Winline -Wno-long-long
    -Wconversion)
add_compile_definitions(_DIOT=1)

add_link_options(
  -Wall
  -specs=nosys.specs
  -Wl,--gc-sections
  -Wl,-Map=output.map
  )

project(${APP} C CXX ASM)

set(CMAKE_CXX_STANDARD 20)

### BSP

include(../bsp/bsp.cmake)

MakeBsp(1)

### END BSP

### bumbleboot

set(BMBOOT_BUILD_PAYLOAD 1)   # TODO: This is *bad*. It should probably be a separate CMakeLists instead.

add_subdirectory(../bmboot ${CMAKE_CURRENT_BINARY_DIR}/bmboot)

### END bumbleboot

set(BMBOOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../bmboot)

include(${BMBOOT_DIR}/cmake/Bmboot.cmake)

# Include directories for headers
include_directories(
    ${CMAKE_SOURCE_DIR}/../lib # all external libraries are stored in this directory
    ${CMAKE_SOURCE_DIR}/../vslib # vslib library is stored in this directory
    ${CMAKE_SOURCE_DIR}/../shared # header files shared between bare-metal and linux are stored in this directory
)

# VSLIB is treated as external library, as it would be in vloop applications
add_subdirectory(../vslib ${CMAKE_CURRENT_BINARY_DIR}/vslib)
add_subdirectory(../lib/fmt ${CMAKE_CURRENT_BINARY_DIR}/fmt)

add_executable(${APP}.elf
  main.cpp
  )

target_include_directories(${APP}.elf PRIVATE
  inc
  public_include
  )


target_link_libraries(${APP}.elf PRIVATE
  vslib
  fmt::fmt
  bmboot_payload_runtime
  fgc4_bsp_el1
  )

Bmboot_PayloadPostBuild(${APP}.elf)

# post-build, automatically run:
# - objcopy to create .bin file
# - objdump to create disassembly
# - (not needed at the moment: mkimage to creat U-Boot image)
set(IMG_FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.img)
if (OVERRIDE_BIN_FILE)
    set(BIN_FILE ${OVERRIDE_BIN_FILE})
else()
    set(BIN_FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin)
endif()

string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UPPER)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/build_flags.txt "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_UPPER}}")    # needed for benchmark framework

############################################################
# Installation
############################################################

set(INSTALL_LIB_DIR ${CMAKE_BINARY_DIR}/bin)

install(TARGETS ${APP}.elf
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
)

############################################################
# Start packaging
############################################################

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
