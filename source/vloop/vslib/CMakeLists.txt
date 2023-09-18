# Collect source files for the vslib code
set(VSLIB_SOURCES
    background/src/background.cpp
    components/src/componentRegistry.cpp
    parameters/src/parameterRegistry.cpp
    # Other vslib source files go here
)

# Create a library from the vslib sources
add_library(vslib ${VSLIB_SOURCES})

# Include directories for vslib
target_include_directories(vslib
    PUBLIC
    background/inc
    components/inc
    parameters/inc
    constants
    utils
    # Other vslib include directories go here
)
target_link_libraries(vslib PRIVATE
  fmt::fmt
  )

############################################################
# Installation
############################################################

set(INSTALL_LIB_DIR ${CMAKE_BINARY_DIR}/lib)

# Specify the installation path for the library
install(TARGETS vslib
    ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
)
