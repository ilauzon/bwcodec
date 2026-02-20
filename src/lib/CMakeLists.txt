file(GLOB sources CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

target_sources(bwcodec
    PRIVATE
        ${sources}
)

file(GLOB headers CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

target_sources(bwcodec
    PRIVATE
        FILE_SET HEADERS
        FILES
            ${headers}
)

target_include_directories(bwcodec
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
