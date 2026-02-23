#include "fileoperations.hpp"
#include <cstdio>
#include <cstdlib>
#include <format>
#include <opencv4/opencv2/opencv.hpp>
#include <string>

static constexpr auto USAGE_ERR_MSG =
    "Usage: bwcodec <encode|decode> <frames_directory> <video_filename>\n";

namespace fs = std::filesystem;

void decode(fs::path videoFilename, fs::path framesDirectory) {
    const auto video = fileoperations::readVideo(videoFilename);
    fileoperations::decodeVideo(video, framesDirectory);
}

void encode(fs::path framesDirectory, fs::path videoFilename) {
    const auto video = fileoperations::encodeVideo(framesDirectory);
    fileoperations::writeVideo(video, videoFilename);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf(USAGE_ERR_MSG);
        return EXIT_FAILURE;
    }

    auto mode_string = std::string(argv[1]);

    auto frames_directory = std::string(argv[2]);

    auto video_filename = std::string(argv[3]);

    if (!fs::is_directory(frames_directory) && fs::exists(frames_directory)) {
        throw std::invalid_argument(
            std::format("{} is not a directory.", frames_directory));
    }

    if (mode_string == "decode") {
        if (!fs::exists(frames_directory)) {
            fs::create_directory(frames_directory);
        }

        if (!fs::exists(video_filename)) {
            throw std::invalid_argument(
                std::format("{} does not exist.", video_filename));
        }

        decode(video_filename, frames_directory);

    } else if (mode_string == "encode") {
        if (!fs::exists(frames_directory)) {
            throw std::invalid_argument(
                std::format("{} does not exist.", frames_directory));
        }

        if (fs::exists(video_filename)) {
            throw std::invalid_argument(
                std::format("{} already exists, choose another file name.",
                            video_filename));
        }

        encode(frames_directory, video_filename);

    } else {
        printf(USAGE_ERR_MSG);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
