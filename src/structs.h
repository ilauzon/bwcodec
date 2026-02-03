#pragma once

#include <cstdint>
#include <vector>

/**
* The version of the software. 
* Consists of 2-byte chunks that describe the current version.
*/
const uint64_t CURRENT_VERSION = 0x00000000000010000;

struct VideoHeader {
    uint64_t version_marker = CURRENT_VERSION;
    uint64_t frame_count;
    uint32_t height;
    uint32_t width;
}; 

struct Frame {
    std::vector<uint8_t> pixels; // Grouped into 8 pixels per vector unit.
};

struct Video {
    VideoHeader header;
    std::vector<Frame> frames;
};
