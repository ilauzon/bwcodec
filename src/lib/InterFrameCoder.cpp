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

};

/**
 * A predicted frame.
 */
struct PFrame {

};

/**
 * Points to an element within either a list of p-frames or i-frames.
 */
struct FrameIndex {
    FrameType type; // whether to look in the list of p-frames or i-frames.
    uint index; // the index of the frame to find within that list.
};

/**
 * A video that uses inter-frame compression.
 */
struct InterCodedVideo {
    VideoHeader header; // video metadata
    std::vector<FrameIndex> frame_ordering; // describes the ordering of the frames. Each element points to either a frame in i_frames or p_frames.
    std::vector<IFrame> i_frames; // the i-frames of the video, in no guaranteed order.
    std::vector<PFrame> p_frames; // the p-frames of the video, in no guaranteed order.
};

/**
 * Performs conversions between an InterCodedVideo and a compressed byte stream.
 */
class InterFrameCoder {
    public:
        static std::vector<std::byte> encode(InterCodedVideo video) {
            throw;
        };
        
        static InterCodedVideo decode(std::vector<std::byte> bytes) {
            throw;
        };
};
