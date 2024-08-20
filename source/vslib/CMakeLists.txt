cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(LIB vslib)

project(${LIB} CXX)

if (NOT DEFINED LIBRARIES_HOME)
  message(FATAL_ERROR "LIBRARIES_HOME path needs to be defined.")
endif()

set(BMBOOT_DIR ${LIBRARIES_HOME}/bmboot)

include(${BMBOOT_DIR}/cmake/Bmboot.cmake)

if (NOT BUILD_TESTS)

  if (NOT DEFINED CMAKE_CXX_COMPILER)
   message(FATAL_ERROR "CMAKE_CXX_COMPILER for bare-metal target needs to be defined.")
  endif()

  ### bumbleboot

  set(BMBOOT_BUILD_PAYLOAD 1)   # TODO: This is *bad*. It should probably be a separate CMakeLists instead.

  add_subdirectory(${BMBOOT_DIR} ${CMAKE_CURRENT_BINARY_DIR}/bmboot)

  ### END bumbleboot

  ### VSlib

  add_compile_options(-Wall -O2 -mtune=cortex-a53 -ffunction-sections -fdata-sections
    -fexceptions
    -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align
    -Wwrite-strings -Wmissing-declarations
    -Wredundant-decls -Wno-long-long
    -Wconversion
  )

  add_link_options(
    -Wall
    -specs=nosys.specs
    -Wl,-gc-sections
  )

  add_subdirectory("${LIBRARIES_HOME}/fmt-8.0.1" ${CMAKE_CURRENT_BINARY_DIR}/fmt)
  add_subdirectory("${LIBRARIES_HOME}/json-schema-validator-2.2.0" ${CMAKE_CURRENT_BINARY_DIR}/json-schema-validator)

  # Collect source files for the vslib code
  set(${LIB}_SOURCES
      components/src/abcToAlphaBetaTransform.cpp
      components/src/abcToDq0Transform.cpp
      components/src/alphaBetaToDq0Transform.cpp
      parameters/src/parameterRegistry.cpp
      background/src/parameterSetting.cpp
      background/src/parameterMap.cpp
      utils/src/vslibMessageQueue.cpp
      ../utils/messageQueue.cpp
  )

  # Create a library from the vslib sources
  add_library(${LIB} ${${LIB}_SOURCES})

  target_link_options(${LIB} PUBLIC
    -specs=nosys.specs
    -Wl,-gc-sections
  )

  # Include directories for vslib
  target_include_directories(${LIB}
      PUBLIC
      .
      background/inc
      components/inc
      parameters/inc
      interrupts/inc
      utils/inc # vslib-specific utilities
      ../utils  # bare-metal shared utilities
      ${LIBRARIES_HOME}/json-3.11.2
      ${LIBRARIES_HOME}/magic_enum-0.9.3
      # Other vslib include directories go here
  )

  target_link_libraries(${LIB} PUBLIC
    fmt::fmt
    nlohmann_json_schema_validator
    bmboot_payload_runtime
    )

  if (PERFORMANCE_TESTS)
    add_definitions(-DPERFORMANCE_TESTS)
  endif()

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

  add_subdirectory("${LIBRARIES_HOME}/json-schema-validator-2.2.0" ${CMAKE_CURRENT_BINARY_DIR}/json-schema-validator)
  # FMT has to be recompiled here due to use of different compiler than in the upstream CMake
  add_subdirectory("${LIBRARIES_HOME}/fmt-8.0.1" ${CMAKE_CURRENT_BINARY_DIR}/fmt)
  include(GoogleTest)
  enable_testing()

  # Include your components and parameters
  add_subdirectory(components)
  add_subdirectory(background)
  add_subdirectory(parameters)
  add_subdirectory(utils)

endif()
