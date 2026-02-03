add_executable(bwcodec)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(bwcodec ${OpenCV_LIBS})

file(GLOB sources "*.cpp")
target_sources(bwcodec
    PRIVATE
        ${sources}
)

file(GLOB headers "*.h" "*.hpp")
target_sources(bwcodec
    PRIVATE
        FILE_SET HEADERS
        FILES
            ${headers}
)

