cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory("${LIBRARIES_HOME}/googletest-1.11.0" "${CMAKE_CURRENT_BINARY_DIR}/googletest")
# FMT has to be recompiled here due to use of different compiler than in the upstream CMake
add_subdirectory("${LIBRARIES_HOME}/fmt-8.0.1" ${CMAKE_CURRENT_BINARY_DIR}/fmt)
include(GoogleTest)

add_executable(utilsTests
  tests/utilsTests.cpp
  tests/errorMessageTest.cpp
  tests/logStringTest.cpp
  tests/ringBufferTest.cpp
  tests/ringBufferAllocatorTest.cpp
  tests/staticJsonTest.cpp
  tests/typeLabelTest.cpp
  tests/warningMessageTest.cpp
)

target_include_directories(utilsTests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${LIBRARIES_HOME}/json-3.11.2)

target_link_libraries(utilsTests PUBLIC ${GTEST_LIBRARIES} GTest::gtest pthread dl fmt::fmt)
target_link_options(utilsTests PUBLIC  -Wl,-dynamic-linker,/my-lib/ld-2.33.so -Wl,-rpath,/my-lib)
target_link_options(utilsTests PRIVATE -static -static-libgcc -static-libstdc++)

if(DEFINED CMAKE_CXX_COMPILER)
    set_target_properties(utilsTests PROPERTIES CXX_COMPILER ${CMAKE_CXX_COMPILER})
endif()
