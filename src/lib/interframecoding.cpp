#include "structs.h"
#include <sys/types.h>
#include <vector>

/**
 * Identifier for whether a frame is an i-frame or p-frame.
 */
enum FrameType {
    I, // i-frame
    P  // p-frame
};

/**
 * An intra-coded frame.
 */
struct IFrame {
    size_t size;
};

/**
 * A predicted frame.
 */
struct PFrame {
    size_t size;
};

/**
 * Points to an element within either a list of p-frames or i-frames.
 */
struct FrameIndex {
    FrameType type; // whether to look in the list of p-frames or i-frames.
    size_t index;   // the index of the frame to find within that list.
};

/**
 * A video that uses inter-frame compression.
 */
struct InterframeCodedVideo {
    VideoHeader header; // video metadata
    std::vector<FrameIndex>
        frame_ordering; // describes the ordering of the frames. Each element
                        // points to either a frame in i_frames or p_frames.
    std::vector<IFrame>
        i_frames; // the i-frames of the video, in no guaranteed order.
    std::vector<PFrame>
        p_frames; // the p-frames of the video, in no guaranteed order.
};

/**
 * Performs conversions between an InterCodedVideo, Video, and a compressed byte
 * stream.
 */
namespace interframecoding {

namespace {

IFrame convertToIFrame(const Frame &frame) { throw; }

PFrame convertToPFrame(const Frame &frame) { throw; }

bool shouldBePFrame(const Frame &frame) {
    const auto iframe = convertToIFrame(frame);
    const auto pframe = convertToPFrame(frame);
    return iframe.size > pframe.size;
}

} // namespace

InterframeCodedVideo toInterframeCoded(Video &video) {
    InterframeCodedVideo coded_video = {};
    coded_video.header = video.header;

    for (int i = 0; i < video.frames.size(); i++) {
        const auto frame = video.frames[i];
        if (shouldBePFrame(frame)) {
            const auto pframe = convertToPFrame(frame);
            coded_video.p_frames.push_back(pframe);
            const auto index = coded_video.p_frames.size() - 1;
            coded_video.frame_ordering.push_back({FrameType::P, index});
        } else {
            const auto iframe = convertToIFrame(frame);
            coded_video.i_frames.push_back(iframe);
            const auto index = coded_video.i_frames.size() - 1;
            coded_video.frame_ordering.push_back({FrameType::I, index});
        }
    }

    return coded_video;
}

std::vector<std::byte> toBytes(InterframeCodedVideo &video) { throw; }

std::vector<std::byte> encode(InterframeCodedVideo &video) { throw; }

InterframeCodedVideo decode(std::vector<std::byte> &bytes) { throw; }

} // namespace interframecoding
