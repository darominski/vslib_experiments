cmake_minimum_required(VERSION 3.12)

if(NOT DEFINED CMAKE_CXX_COMPILER)
  message(FATAL_ERROR "CMAKE_CXX_COMPILER must be set to linux-gnu compiler")
endif()

project(LinuxParameterSetting CXX)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_CXX_STANDARD 20)

### bumbleboot

set(BMBOOT_DIR ${LIBRARIES_HOME}/bmboot)

include(${BMBOOT_DIR}/cmake/Bmboot.cmake)

set(BMBOOT_BUILD_PAYLOAD 0)   # TODO: This is *bad*. It should probably be a separate CMakeLists instead.
add_subdirectory(${BMBOOT_DIR} ${CMAKE_CURRENT_BINARY_DIR}/bmboot)

add_subdirectory("${LIBRARIES_HOME}/fmt-8.0.1" ${CMAKE_CURRENT_BINARY_DIR}/fmt)

# Compiler flags and warnings
add_compile_options(-O2 -mtune=cortex-a53 -ffunction-sections -fdata-sections -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align
    -Wwrite-strings -Wmissing-declarations
    -Wredundant-decls -Wno-long-long
    -Wconversion)

# Linker flags and warnings
add_link_options(
  -Wall
  -Wl,--gc-sections
)

set(loader_SOURCES
    loader.cpp
    ${CMAKE_SOURCE_DIR}/../utils/messageQueue.cpp
    ${CMAKE_SOURCE_DIR}/../vslib/utils/src/vslibMessageQueue.cpp
)

# Create executables
add_executable(loader
  ${loader_SOURCES}
  )

# Add external libraries
include_directories(
    ${CMAKE_SOURCE_DIR}/../lib # all external libraries are stored in this directory
    ${LIBRARIES_HOME}/json-3.11.2
    ${CMAKE_SOURCE_DIR}/../utils # all general fgc4-utils are stored in this directory
    ${CMAKE_SOURCE_DIR}/../vslib/utils/inc # all vslib-specific utils are stored in this directory
    ${LIBRARIES_HOME}/bmboot/include # all public bmboot includes are stored in this directory
)

target_compile_features(loader PUBLIC cxx_std_20)

target_link_options(loader PRIVATE -static -static-libgcc -static-libstdc++)

target_link_libraries(loader
  PRIVATE
  fmt::fmt
  bmboot_manager
)
