cmake_minimum_required(VERSION 3.15)
project(coglux VERSION 0.1.0 LANGUAGES CXX)

option(BUILD_TESTS "Build CogLux tests" ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(coglux STATIC
    src/coglux.cpp
)

target_include_directories(coglux PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(coglux PUBLIC cxx_std_11)
add_library(coglux::coglux ALIAS coglux)

# Backwards-compatible header-only target name used by earlier cogpy-hpp templates.
add_library(cog INTERFACE)
target_include_directories(cog INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

install(TARGETS coglux
    EXPORT cogluxTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY include/coglux DESTINATION include)
install(DIRECTORY include/cog DESTINATION include OPTIONAL)
