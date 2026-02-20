#include "structs.h"
#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <vector>

namespace fileoperations {

std::vector<cv::Mat> readImages(std::filesystem::path imagesDirectory);

void writeImages();

Video readVideo(std::filesystem::path videoFilename);

void writeBytes(const std::vector<std::byte> bytes,
                std::filesystem::path newFilename);

} // namespace fileoperations
