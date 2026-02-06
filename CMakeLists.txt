cmake_minimum_required(VERSION 3.23)
project(bwcodec)
add_subdirectory(src)
target_compile_features(bwcodec PUBLIC cxx_std_23)
target_compile_options(bwcodec 
    PRIVATE 
        -g
        -fno-omit-frame-pointer
        -mno-omit-leaf-frame-pointer
)
