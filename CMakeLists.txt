cmake_minimum_required(VERSION 3.28)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER clang++)
project(bwcodec)

add_subdirectory(src)
target_compile_options(bwcodec 
    PRIVATE 
        -g
        -fno-omit-frame-pointer
        -mno-omit-leaf-frame-pointer
)


if (PROJECT_IS_TOP_LEVEL AND UNIX)
    # Create symlink to compile_commands.json for IDE to pick it up
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )
endif()
