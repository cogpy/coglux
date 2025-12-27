cmake_minimum_required(VERSION 3.15)
project(coglux VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# RPATH settings for proper shared library loading
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_RPATH "${CMAKE_BINARY_DIR}")
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# Options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# Main library
add_library(coglux
    src/coglux.cpp
)

target_include_directories(coglux PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(coglux PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS coglux
    EXPORT coglux-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT coglux-targets
    FILE coglux-config.cmake
    NAMESPACE coglux::
    DESTINATION lib/cmake/coglux
)
