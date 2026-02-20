#include "structs.h"
#include <filesystem>
#include <fstream>
#include <opencv2/core/mat.hpp>
#include <vector>

namespace fileoperations {

std::vector<cv::Mat> readImages(std::filesystem::path imagesDirectory) {}

void writeImages() {}

Video readVideo(std::filesystem::path videoFilename) {}

void writeBytes(const std::vector<std::byte> bytes,
                std::filesystem::path newFilename) {
    std::ofstream videoFileStream(newFilename, std::ios::binary);
    if (videoFileStream.is_open()) {
        videoFileStream.write(reinterpret_cast<const char *>(bytes.data()),
                              bytes.size());
        videoFileStream.close();
    }
}

} // namespace fileoperations
