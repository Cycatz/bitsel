function(make_example target)
    add_executable("example-${target}" ${target}.cc)
    set_target_properties("example-${target}" PROPERTIES OUTPUT_NAME "${target}")
    target_link_libraries("example-${target}" PRIVATE bitslice)
    target_compile_options(
        "example-${target}" PRIVATE
        -g
        -Werror
        -Wall
        -Wextra
        -Wno-noexcept-type # GCC
    )
endfunction()

make_example(utils)
make_example(des)
