#include "structs.h"
#include <vector>
class VideoCoder {
    public:
        std::vector<std::byte> encode(Video & video);
        Video decode(std::vector<std::byte> & bytes);
    private:
        int getBytesPerFrame(const VideoHeader & video_header);

};
