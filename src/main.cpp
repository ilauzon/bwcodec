#include "ProgressBar.hpp"
#include "fileoperations.hpp"
#include "opencv2/core/hal/interface.h"
#include "opencv2/core/mat.hpp"
#include "opencv2/imgcodecs.hpp"
#include "structs.h"
#include "videobyteconversions.hpp"
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <opencv4/opencv2/opencv.hpp>
#include <string>
#include <vector>

#define USAGE_ERR_MSG                                                          \
    "Usage: bwcodec <encode|decode> <frames_directory> <video_filename>\n"

namespace fs = std::filesystem;

uint64_t getBytesPerFrame(const VideoHeader &video_header) {
    return std::ceil(
        static_cast<double>(video_header.width * video_header.height) / 8);
}

/*
 * Convert the frames of a video contained within a directory to a Video struct.
 */
Video convertImagesToVideo(const fs::path &frames_directory) {
    std::vector<fs::path> image_paths;
    const std::string extension = ".png";

    for (auto const &entry : fs::directory_iterator(frames_directory)) {
        if (fs::is_regular_file(entry) &&
            entry.path().extension() == extension) {
            image_paths.emplace_back(entry.path());
        }
    }

    if (image_paths.size() == 0) {
        throw std::invalid_argument(std::format("{} contains zero PNG images",
                                                frames_directory.string()));
    }

    cv::Mat first_frame = cv::imread(image_paths[0], cv::IMREAD_GRAYSCALE);
    Video video = {};
    video.header = {};
    video.header.height = first_frame.rows;
    video.header.width = first_frame.cols;
    video.header.frame_count = image_paths.size();

    printf("Frame count: %ld\n", video.header.frame_count);
    printf("Frame height: %d\n", video.header.height);
    printf("Frame width: %d\n", video.header.width);

    const auto elements_per_frame = getBytesPerFrame(video.header);

    auto progressBar =
        ProgressBar("Reading images from disk", image_paths.size());
    for (auto const &image_path : image_paths) {
        progressBar.update_increment();

        cv::Mat img = cv::imread(image_path, cv::IMREAD_GRAYSCALE);

        if (img.rows != first_frame.rows || img.cols != first_frame.cols) {
            throw std::invalid_argument(std::format(
                "Dimension mismatch: image height {} != {}, or width {} != {}",
                img.rows, first_frame.rows, img.cols, first_frame.cols));
        }

        uint8_t pixel_8_group = 0;
        uint8_t pixel_counter = 0;
        const uint8_t expected_bit_depth = 8;
        const uint64_t max_intensity = pow(2, expected_bit_depth);

        Frame frame = {};

        for (int row = 0; row < video.header.height; row++) {
            for (int col = 0; col < video.header.width; col++) {

                if (pixel_counter == 8) {
                    frame.pixels.push_back(
                        static_cast<std::byte>(pixel_8_group));
                    pixel_counter = 0;
                }

                uint8_t intensity = img.at<uchar>(row, col);
                bool is_white_pixel = intensity > max_intensity / 2;
                pixel_8_group = (pixel_8_group << 1) | is_white_pixel;
                pixel_counter++;
            }
        }

        if (pixel_counter > 0) {
            pixel_8_group <<= (8 - pixel_counter);
            frame.pixels.push_back(static_cast<std::byte>(pixel_8_group));
        }

        video.frames.push_back(frame);
    }
    std::printf("\n"); // finish progress bar

    return video;
}

/**
 * Convert a Video struct to a series of frames, saved as images to the given
 * directory.
 */
void convertVideoToImages(const Video &video,
                          const fs::path &frames_directory) {
    int image_type = CV_8UC1;
    constexpr uint8_t DEFAULT_COLOR = 64;
    cv::Scalar initial_color(DEFAULT_COLOR);
    std::vector<cv::Mat> images;

    auto progressBar =
        ProgressBar("Writing frames to disk", video.header.frame_count * 2);
    for (int frame_idx = 0; frame_idx < video.header.frame_count; frame_idx++) {
        progressBar.update_increment();
        cv::Mat image(video.header.height, video.header.width, image_type,
                      initial_color);
        const Frame &frame = video.frames[frame_idx];

        int pixel_counter = 0;
        for (int pixel_idx = 0; pixel_idx < frame.pixels.size(); pixel_idx++) {
            uint8_t pixel_8_group =
                static_cast<uint8_t>(frame.pixels[pixel_idx]);

            for (int i = 7; i >= 0; i--) {
                bool pixel_is_on = (pixel_8_group & ((uint8_t)1 << i)) >> i;

                int row = pixel_counter / video.header.width;
                int col = pixel_counter % video.header.width;

                image.at<uchar>(row, col) = pixel_is_on ? 255 : 0;

                pixel_counter++;
                if (pixel_counter >= video.header.height * video.header.width) {
                    goto endframe;
                }
            }
        }
    endframe:

        images.push_back(image);
    }

    for (int i = 0; i < images.size(); i++) {
        progressBar.update_increment();
        const fs::path file_path =
            frames_directory / std::format("frame{}.png", i);
        cv::imwrite(file_path, images[i]);
    }
    std::printf("\n"); // finish progress bar
}

void decode(fs::path videoFilename, fs::path framesDirectory) {
    std::ifstream videoFileStream(videoFilename, std::ios::binary);

    if (!videoFileStream) {
        std::cerr << "Error opening file: " << videoFilename << std::endl;
        exit(1);
    }

    std::ifstream ifs(videoFilename, std::ios::binary | std::ios::ate);

    // Get file size
    auto size = ifs.tellg();
    std::vector<std::byte> bytes(size);

    // Seek back to the beginning and read directly into the vector
    ifs.seekg(std::ios::beg);
    ifs.read(reinterpret_cast<char *>(bytes.data()), size);

    videoFileStream.close();

    const auto video = videobyteconversions::toVideo(bytes);
    convertVideoToImages(video, framesDirectory);
}

void encode(fs::path framesDirectory, fs::path videoFilename) {
    Video video = convertImagesToVideo(framesDirectory);
    const auto videoBytes = videobyteconversions::toBytes(video);
    fileoperations::writeBytes(videoBytes, videoFilename);
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
