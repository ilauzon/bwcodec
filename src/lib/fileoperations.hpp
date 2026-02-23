#include "structs.h"
#include <filesystem>
#include <opencv2/core/mat.hpp>

namespace fs = std::filesystem;

namespace fileoperations {

Video encodeVideo(const fs::path &imagesDirectory);

void decodeVideo(const Video &video, const fs::path &frames_directory);

Video readVideo(const fs::path &videoFilename);

void writeVideo(const Video &video, const fs::path newFilename);

} // namespace fileoperations
