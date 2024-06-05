set(APP componentTests)

add_executable(${APP}
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/boxFilterTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/componentTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/componentTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/componentArrayTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/firFilterTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/iirFilterTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/lookupTableTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/periodicLookupTableTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/sinLookupTableTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/cosLookupTableTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/limitIntegralTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/limitRangeTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/limitRateTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/limitRmsTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/pidTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/pidRstTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/rstControllerTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/rstTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/parkTransformTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rstController.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/parkTransform.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../parameters/src/parameterRegistry.cpp
)

# TODO: the messy include structure below could be avoided if the entire vslib was compiled
# with the same compiler compatible with the gtest, e.g. aarch64-none-linux-gnu-g++,
# and then linked as library for testing purposes

target_include_directories(${APP} PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../parameters/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils        # FGC4-general utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/../utils/inc       # VSlib-only utilities
    ${LIBRARIES_HOME}/json-3.11.2
    ${LIBRARIES_HOME}/magic_enum-0.9.3
)

target_link_libraries(${APP} PUBLIC ${GTEST_LIBRARIES} GTest::gtest pthread dl fmt::fmt)
target_link_options(${APP} PUBLIC  -Wl,-dynamic-linker,/my-lib/ld-2.33.so -Wl,-rpath,/my-lib)
target_link_options(${APP} PRIVATE -static -static-libgcc -static-libstdc++)

add_custom_command(
        TARGET ${APP} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/inputs
                $<TARGET_FILE_DIR:${APP}>/inputs)
