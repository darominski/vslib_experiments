cmake_minimum_required(VERSION 3.17)

set(LIB vslib)

project(${LIB} CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if (NOT DEFINED LIBRARIES_HOME)
  message(FATAL_ERROR "LIBRARIES_HOME path needs to be defined.")
endif()

add_compile_options(-Wall -O0 -mtune=cortex-a53 -ffunction-sections -fdata-sections
  -fexceptions
  -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align
  -Wwrite-strings -Wmissing-declarations
  -Wredundant-decls -Wno-long-long
  -Wconversion
)

if (NOT BUILD_TESTS)

  add_link_options(
    -Wall
    -specs=nosys.specs
    -Wl,-gc-sections
  )

  if (NOT DEFINED CMAKE_CXX_COMPILER)
   message(FATAL_ERROR "CMAKE_CXX_COMPILER for bare-metal target needs to be defined.")
  endif()

  add_subdirectory("${LIBRARIES_HOME}/fmt-8.0.1" ${CMAKE_CURRENT_BINARY_DIR}/fmt)

  # Collect source files for the vslib code
  set(${LIB}_SOURCES
      background/src/background.cpp
      components/src/componentRegistry.cpp
      parameters/src/parameterRegistry.cpp
     # Other vslib source files go here
  )

  # Create a library from the vslib sources
  add_library(${LIB} ${${LIB}_SOURCES})

  # Include directories for vslib
  target_include_directories(${LIB}
      PUBLIC
      background/inc
      components/inc
      parameters/inc
      ../utils
      ${LIBRARIES_HOME}/json-3.11.2
      ${LIBRARIES_HOME}/magic_enum-0.9.3
      # Other vslib include directories go here
  )

  target_link_libraries(${LIB} PRIVATE
    fmt::fmt
    )

endif()

############################################################
# Installation
############################################################

if (NOT BUILD_TESTS)

  set(INSTALL_LIB_DIR ${CMAKE_BINARY_DIR}/lib)

  # Specify the installation path for the library
  install(TARGETS vslib
      ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  )
endif()

############################################################
# Tests
############################################################

if(BUILD_TESTS)

  add_subdirectory("${LIBRARIES_HOME}/googletest-1.11.0" "${CMAKE_CURRENT_BINARY_DIR}/googletest")
  # FMT has to be recompiled here due to use of different compiler than in the upstream CMake
  add_subdirectory("${LIBRARIES_HOME}/fmt-8.0.1" ${CMAKE_CURRENT_BINARY_DIR}/fmt)
  include(GoogleTest)
  enable_testing()

  # Include your components and parameters
  add_subdirectory(components)
  add_subdirectory(parameters)

endif()
