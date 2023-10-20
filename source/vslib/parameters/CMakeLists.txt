set(APP parameterTests)

add_executable(${APP}
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/parameterTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/parameterTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/parameterRegistryTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/parameterRegistry.cpp
)

# TODO: the messy include structure below could be avoided if the entire vslib was compiled
# with the same compiler compatible with the gtest, e.g. aarch64-none-linux-gnu-g++,
# and then linked as library for testing purposes

target_include_directories(${APP} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../components/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils
    ${LIBRARIES_HOME}/json-3.11.2
    ${LIBRARIES_HOME}/magic_enum-0.9.3
)

target_link_libraries(${APP} PUBLIC ${GTEST_LIBRARIES} GTest::gtest pthread dl fmt::fmt)
target_link_options(${APP} PUBLIC  -Wl,-dynamic-linker,/my-lib/ld-2.33.so -Wl,-rpath,/my-lib)
target_link_options(${APP} PRIVATE -static -static-libgcc -static-libstdc++)
