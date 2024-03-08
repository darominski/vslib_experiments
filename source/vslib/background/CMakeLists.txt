set(APP backgroundTests)

add_executable(${APP}
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/backgroundTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/parameterMapTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/parameterSettingTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/parameterMap.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/parameterSetting.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/messageQueue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../utils/src/vslibMessageQueue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../components/src/componentRegistry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../parameters/src/parameterRegistry.cpp
)

# TODO: the messy include structure below could be avoided if the entire vslib was compiled
# with the same compiler compatible with the gtest, e.g. aarch64-none-linux-gnu-g++,
# and then linked as library for testing purposes

target_include_directories(${APP} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../components/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../parameters/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils        # FGC4-general utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/../utils/inc       # VSlib-only utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${LIBRARIES_HOME}/json-3.11.2
    ${LIBRARIES_HOME}/magic_enum-0.9.3
)

target_link_libraries(${APP} PUBLIC ${GTEST_LIBRARIES} GTest::gtest GTest::gmock pthread dl fmt::fmt nlohmann_json_schema_validator)
target_link_options(${APP} PUBLIC  -Wl,-dynamic-linker,/my-lib/ld-2.33.so -Wl,-rpath,/my-lib)
target_link_options(${APP} PRIVATE -static -static-libgcc -static-libstdc++)
