function(setup_clang_tidy)
    find_program(
        CLANG_TIDY_EXE
        NAMES "clang-tidy"
        DOC "Path to clang-tidy executable"
    )
    if(USE_CLANG_TIDY)
        if(CLANG_TIDY_EXE)
            set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}")
            if(LINT_AS_ERRORS)
                set(DO_CLANG_TIDY "${DO_CLANG_TIDY}" "-warnings-as-errors=*")
            endif()
            message(STATUS "use clang-tidy with DO_CLANG_TIDY: ${DO_CLANG_TIDY}")
        else()
            message(FATAL_ERROR "USE_CLANG_TIDY is on but clang-tidy is not found")
        endif()
    else()
        message(STATUS "not use clang-tidy")
    endif()
endfunction()
