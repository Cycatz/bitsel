cmake_minimum_required(VERSION 3.10)
project(Bitslice
    VERSION 0.1.0
    LANGUAGES CXX
    )

# compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")
option(DEBUG_SYMBOL "add debug information" ON)
message(STATUS "DEBUG_SYMBOL: ${DEBUG_SYMBOL}")
if(DEBUG_SYMBOL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

set(BITSLICE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${BITSLICE_INCLUDE_DIR})

set(BITSLICE_HEADERS include/bitslice.hpp)
set(BITSLICE_SOURCES src/bitslice.cc)
add_library(bitslice SHARED ${BITSLICE_SOURCES})

set(BITSLICE_TEST_SOURCES test/bitslice_test.cc)
set(BITSLICE_EXAMPLE_SOURCES example/example.cc)

target_compile_options(
    bitslice PRIVATE
    -Werror
    -Wall
    -Wextra
    -Wno-noexcept-type # GCC
)



####################
# clang-tidy
####################

option(USE_CLANG_TIDY "use clang-tidy" ON)
include(cmake/clang_tidy.cmake)
setup_clang_tidy()
if(CLANG_TIDY_EXE AND USE_CLANG_TIDY)
    set_target_properties(
        bitslice PROPERTIES
        CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
    )
endif()


####################
# clang-format
####################
include(cmake/clang_format.cmake)
add_clang_format_target()


####################
# test
####################
add_subdirectory(test)


####################
# example
####################
add_subdirectory(example)
