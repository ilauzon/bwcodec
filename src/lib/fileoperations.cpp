#include "structs.h"
#include "videobyteconversions.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>

namespace fs = std::filesystem;

namespace fileoperations {

namespace {

uint64_t getBytesPerFrame(const VideoHeader &video_header) {
    return std::ceil(
        static_cast<double>(video_header.width * video_header.height) / 8);
}

} // namespace

Video encodeVideo(const fs::path &frames_directory) {
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

    for (auto const &image_path : image_paths) {
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
    return video;
}

void decodeVideo(const Video &video, const fs::path &frames_directory) {
    int image_type = CV_8UC1;
    constexpr uint8_t DEFAULT_COLOR = 64;
    cv::Scalar initial_color(DEFAULT_COLOR);
    std::vector<cv::Mat> images;

    for (int frame_idx = 0; frame_idx < video.header.frame_count; frame_idx++) {
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
        const fs::path file_path =
            frames_directory / std::format("frame{}.png", i);
        cv::imwrite(file_path, images[i]);
    }
}

Video readVideo(const fs::path &videoFilename) {
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
    return video;
}

void writeVideo(const Video &video, const std::filesystem::path newFilename) {
    const auto bytes = videobyteconversions::toBytes(video);
    std::ofstream videoFileStream(newFilename, std::ios::binary);
    if (videoFileStream.is_open()) {
        videoFileStream.write(reinterpret_cast<const char *>(bytes.data()),
                              bytes.size());
        videoFileStream.close();
    }
}

} // namespace fileoperations
