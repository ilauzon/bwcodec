module;
#include "structs.h"
#include <cmath>
#include <cstring>
#include <format>
#include <stdexcept>
#include <vector>

export module VideoByteCoding;

namespace VideoByteCoding {

namespace {
int getBytesPerFrame(const VideoHeader &video_header) {
    return std::ceil(
        static_cast<double>(video_header.width * video_header.height) / 8);
}
} // namespace

export std::vector<std::byte> toBytes(Video &video) {
    std::vector<std::byte> bytes = {};

    // append header
    const std::byte *header_byte_pointer =
        reinterpret_cast<const std::byte *>(&video.header);
    bytes.insert(bytes.end(), header_byte_pointer,
                 header_byte_pointer + sizeof(video.header));

    // append frames
    for (auto &frame : video.frames) {
        const auto frame_pixels_pointer =
            reinterpret_cast<std::byte *>(frame.pixels.data());
        bytes.insert(bytes.end(), frame_pixels_pointer,
                     frame_pixels_pointer + frame.pixels.size());
    }
    return bytes;
}

export Video toVideo(std::vector<std::byte> &bytes) {
    Video video = {};

    // copy bytes to header
    if (bytes.size() < sizeof(VideoHeader)) {
        constexpr auto message = "Video header of length {} bytes is not long "
                                 "enough to be cast to struct of {} bytes.";
        throw std::length_error(
            std::format(message, bytes.size(), sizeof(VideoHeader)));
    }
    video.header = {};
    memcpy(&video.header, bytes.data(), sizeof(video.header));

    // copy bytes to frames
    size_t num_frames = video.header.frame_count;
    video.frames.resize(num_frames);

    int bytes_per_frame = getBytesPerFrame(video.header);

    for (size_t i = 0; i < num_frames; i++) {
        int byte_index = sizeof(video.header) + i * bytes_per_frame;
        Frame frame = {};

        auto begin_iterator = bytes.begin() + byte_index;
        auto end_iterator = begin_iterator + bytes_per_frame;

        frame.pixels.assign(begin_iterator, end_iterator);
        video.frames[i] = frame;
    }

    return video;
}
} // namespace VideoByteCoding
