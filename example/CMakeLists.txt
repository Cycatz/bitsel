add_executable(
    example
    example.cc
    )

target_link_libraries(example
  PRIVATE
    bitslice
  )

target_compile_options(
    example PRIVATE
    -Werror
    -Wall
    -Wextra
    -Wno-noexcept-type # GCC
)
