enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.11.0.tar.gz
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
    unit_tests
    bitslice_test.cc
    )

target_link_libraries(unit_tests
  PRIVATE
    bitslice
    gtest_main
  )

target_compile_options(
    unit_tests PRIVATE
    -Werror
    -Wall
    -Wextra
    -Wno-noexcept-type # GCC
)

# automatic discovery of unit tests
include(GoogleTest)
gtest_discover_tests(unit_tests
  PROPERTIES
    LABELS "unit"
  DISCOVERY_TIMEOUT  # how long to wait (in seconds) before crashing
    240
  )
