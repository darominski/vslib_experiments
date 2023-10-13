cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory("${LIBRARIES_HOME}/googletest-1.11.0" "${CMAKE_CURRENT_BINARY_DIR}/googletest")
# FMT has to be recompiled here due to use of different compiler than in the upstream CMake
add_subdirectory("${LIBRARIES_HOME}/fmt-8.0.1" ${CMAKE_CURRENT_BINARY_DIR}/fmt)
include(GoogleTest)

add_executable(componentTests
  tests/componentTests.cpp
  tests/componentTest.cpp
  tests/componentRegistryTest.cpp
  src/componentRegistry.cpp
  ../parameters/src/parameterRegistry.cpp
)

# TODO: the messy include structure below could be avoided if the entire vslib was compiled
# with the same compiler compatible with the gtest, e.g. aarch64-none-linux-gnu-g++,
# and then linked as library for testing purposes

target_include_directories(componentTests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../parameters/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils
    ${LIBRARIES_HOME}/json-3.11.2
    ${LIBRARIES_HOME}/magic_enum-0.9.3
)

target_link_libraries(componentTests PUBLIC ${GTEST_LIBRARIES} GTest::gtest pthread dl fmt::fmt)
target_link_options(componentTests PUBLIC  -Wl,-dynamic-linker,/my-lib/ld-2.33.so -Wl,-rpath,/my-lib)
target_link_options(componentTests PRIVATE -static -static-libgcc -static-libstdc++)

if(DEFINED CMAKE_CXX_COMPILER)
    set_target_properties(componentTests PROPERTIES CXX_COMPILER ${CMAKE_CXX_COMPILER})
endif()
