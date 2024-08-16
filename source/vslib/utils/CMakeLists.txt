set(APP utilsTests)

add_executable(${APP}
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/utilsTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/containerSearchTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/fixedPointTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/histogramTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/statisticsTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/typeVerificationTest.cpp
)

target_include_directories(${APP} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/inc
  ${CMAKE_CURRENT_SOURCE_DIR}/../components/inc
  ${CMAKE_CURRENT_SOURCE_DIR}/../parameters/inc
  ${CMAKE_CURRENT_SOURCE_DIR}/../background/inc
  ${CMAKE_CURRENT_SOURCE_DIR}/../../utils
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${LIBRARIES_HOME}/json-3.11.2
  ${LIBRARIES_HOME}/magic_enum-0.9.3
)

target_link_libraries(${APP} PRIVATE ${GTEST_LIBRARIES} GTest::gtest pthread dl fmt::fmt nlohmann_json_schema_validator)
target_link_options(${APP} PRIVATE  -Wl,-dynamic-linker,/my-lib/ld-2.33.so -Wl,-rpath,/my-lib)
target_link_options(${APP} PRIVATE -static -static-libgcc -static-libstdc++)

if(DEFINED CMAKE_CXX_COMPILER)
    set_target_properties(${APP} PROPERTIES CXX_COMPILER ${CMAKE_CXX_COMPILER})
endif()
