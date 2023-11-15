cmake_minimum_required(VERSION 3.12)

if(NOT DEFINED CMAKE_CXX_COMPILER)
  message(FATAL_ERROR "CMAKE_CXX_COMPILER must be set to linux-gnu compiler")
endif()

project(LinuxParameterSetting CXX)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_CXX_STANDARD 20)

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

set(REMOTE_SOURCES
    remote.cpp
)

set(REMOTE_USER_SOURCES
    remote_user.cpp
)

# Create executables
add_executable(remote
  ${REMOTE_SOURCES}
  )

  add_executable(remote_user
  ${REMOTE_USER_SOURCES}
  )

# Add external libraries
include_directories(
    ${CMAKE_SOURCE_DIR}/../lib # all external libraries are stored in this directory
    ${LIBRARIES_HOME}/json-3.11.2
    ${CMAKE_SOURCE_DIR}/../utils # all external libraries are stored in this directory
    ${CMAKE_SOURCE_DIR}/../vslib/utils/inc # all external libraries are stored in this directory
)

add_subdirectory("${LIBRARIES_HOME}/fmt-8.0.1" ${CMAKE_CURRENT_BINARY_DIR}/fmt)

target_compile_features(remote PUBLIC cxx_std_20)
target_compile_features(remote_user PUBLIC cxx_std_20)

target_link_options(remote PRIVATE -static -static-libgcc -static-libstdc++)
target_link_options(remote_user PRIVATE -static -static-libgcc -static-libstdc++)

target_link_libraries(remote PRIVATE fmt::fmt)
target_link_libraries(remote_user PRIVATE fmt::fmt)
