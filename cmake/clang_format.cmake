function(add_clang_format_target)
    if(NOT ${PROJECT_NAME}_CLANG_FORMAT_BINARY)
			find_program(${PROJECT_NAME}_CLANG_FORMAT_BINARY clang-format)
    endif()

    if(${PROJECT_NAME}_CLANG_FORMAT_BINARY)
            add_custom_target(clang-format
                    COMMAND ${${PROJECT_NAME}_CLANG_FORMAT_BINARY}
                    -i ${BITSLICE_HEADERS} ${BITSLICE_SOURCES} ${BITSLICE_TEST_SOURCES}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

			message(STATUS "Format the project using the `clang-format` target (i.e: cmake --build build --target clang-format).\n")
    endif()
endfunction()
