#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <format>
#include <string>
#include <vector>
#include <opencv4/opencv2/opencv.hpp>
#include "opencv2/core/hal/interface.h"
#include "opencv2/core/mat.hpp"
#include "opencv2/imgcodecs.hpp"
#include "structs.h"

#define USAGE_ERR_MSG "Usage: bwcodec <encode|decode> <frames_directory> <video_filename>\n"

namespace fs = std::filesystem;

/**
 * Convert the frames of a video contained within a directory to a Video struct.
 */
Video convertImagesToVideo(const fs::path & frames_directory) {
    std::vector<fs::path> image_paths;
    const std::string extension = ".png";

    if (!fs::exists(frames_directory) || !fs::is_directory(frames_directory)) {
        constexpr auto message = "{} does not exist or is not a directory.";
        throw std::invalid_argument(std::format(message, frames_directory.string()));
    }

    for (auto const & entry : fs::directory_iterator(frames_directory)) {
        if (fs::is_regular_file(entry) && entry.path().extension() == extension) {
            image_paths.emplace_back(entry.path());
        }
    }

    if (image_paths.size() == 0) {
        throw std::invalid_argument(std::format(
                    "{} contains zero PNG images", 
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

    const uint64_t elements_per_frame = std::ceil(
            static_cast<double>(video.header.width * video.header.height) / 8);

    for (auto const & image_path : image_paths) {
        cv::Mat img = cv::imread(image_path, cv::IMREAD_GRAYSCALE);

        if (img.rows != first_frame.rows || img.cols != first_frame.cols) {
            throw std::invalid_argument(std::format(
                    "Dimension mismatch: image height {} != {}, or width {} != {}",
                    img.rows, first_frame.rows, img.cols, first_frame.cols
                    ));
        }

        uint8_t pixel_8_group = 0;
        uint8_t pixel_counter = 0;
        const uint8_t expected_bit_depth = 8;
        const uint64_t max_intensity = pow(2, expected_bit_depth);

        Frame frame = {};

        for (int row = 0; row < video.header.height; row++) {
            for (int col = 0; col < video.header.width; col++) {

                if (pixel_counter == 8) {
                    frame.pixels.push_back(pixel_8_group);
                    pixel_counter = 0;
                }

                uint8_t intensity = img.at<uchar>(row, col);
                bool is_white_pixel = intensity > max_intensity / 2;
                pixel_8_group = (pixel_8_group << 1) | is_white_pixel;
                pixel_counter++;

            }
        }
        video.frames.push_back(frame);
    }

    return video;
}

/**
 * Convert a Video struct to a series of frames, saved as images to the given directory.
 */
void convertVideoToImages(const Video& video, const fs::path & frames_directory) {
    int image_type = CV_8UC1;
    cv::Scalar initial_color(128);
    std::vector<cv::Mat> images;

    for (int frame_idx = 0; frame_idx < video.header.frame_count; frame_idx++) {
        cv::Mat image(
                video.header.height, 
                video.header.width, 
                image_type, 
                initial_color);
        const Frame & frame = video.frames[frame_idx];

        int pixel_counter = 0;
        for (int pixel_idx = 0; pixel_idx < frame.pixels.size(); pixel_idx++) {
            uint8_t pixel_8_group = frame.pixels[pixel_idx];
            
            for (int i = 7; i >= 0; i--) {
                bool pixel_is_on = (pixel_8_group & ((uint8_t)1 << i)) >> i;

                int row = pixel_counter / video.header.width;
                int col = pixel_counter % video.header.width;

                image.at<uchar>(row, col) = pixel_is_on ? 255 : 0;

                pixel_counter++;
                if (pixel_counter > video.header.height * video.header.width) {
                    goto endframe;
                }
            }
        }
        endframe:

        images.push_back(image);
    }

    for (int i = 0; i < images.size(); i++) {
        const fs::path file_path = frames_directory / std::format("frame{}.png", i);
        cv::imwrite(file_path, images[i]);
    }
}

/**
 * Convert a vector of bytes to a Video struct.
 * @param bytes the bytes to convert.
 * @return the populated Video struct.
 */
Video convertBytesToVideo(const std::vector<uint8_t> &bytes) {
    Video video = {};

    // copy bytes to header
    if (bytes.size() < sizeof(VideoHeader)) {
        constexpr auto message = "Video header of length {} bytes is not long enough to be cast to struct of {} bytes.";
        throw std::length_error(std::format(message, bytes.size(), sizeof(VideoHeader)));
    }
    video.header = {};
    memcpy(&video.header, bytes.data(), sizeof(video.header));
    
    // copy bytes to frames
    uint num_frames = ((bytes.size() - sizeof(video.header)) / sizeof(Frame));
    video.frames.resize(num_frames);

    for (uint i = 0; i < num_frames; i++) {
        int byte_index = i * sizeof(Frame) + sizeof(VideoHeader);
        Frame frame = {};

        auto begin_iterator = bytes.begin() + byte_index;
        auto end_iterator = begin_iterator + sizeof(Frame);

        frame.pixels.assign(begin_iterator, end_iterator);
        video.frames[i] = frame;
    }

    return video;
}

/**
 * Convert a Video struct to a vector of bytes.
 *
 * @param video the Video struct to convert.
 * @return the vector of bytes.
 */
std::vector<uint8_t> convertVideoToBytes(const Video &video) {
    std::vector<uint8_t> bytes = {};

    // append header
    const uint8_t* header_byte_pointer = reinterpret_cast<const uint8_t*>(&video.header);
    bytes.insert(bytes.end(), header_byte_pointer, header_byte_pointer + sizeof(video.header));

    // append frames
    for (auto frame : video.frames) {
        const uint8_t* frame_pixels_pointer = reinterpret_cast<const uint8_t*>(&frame.pixels);
        bytes.insert(bytes.end(), frame_pixels_pointer, frame_pixels_pointer + frame.pixels.size());
    }
    return bytes;
}

void decode(fs::path videoFilename, fs::path framesDirectory) 
{
    std::ifstream videoFileStream(videoFilename, std::ios::binary);
    videoFileStream.close();
}

void encode(fs::path framesDirectory, fs::path videoFilename) 
{
    // std::ofstream videoFileStream(videoFilename, std::ios::binary);
    // videoFileStream.close();
    const Video video = convertImagesToVideo(framesDirectory);
    convertVideoToImages(video, framesDirectory);
    
}

int main(int argc, char* argv[]) 
{
    if (argc != 4) 
    {
        printf(USAGE_ERR_MSG);
        return EXIT_FAILURE;
    }

    auto modeString = std::string(argv[1]);

    auto framesDirectory = std::string(argv[2]);

    auto videoFilename = std::string(argv[3]);

    if (modeString == "decode") 
    {
        decode(videoFilename, framesDirectory);

    } else if (modeString == "encode") 
    {
        encode(framesDirectory, videoFilename);

    } else 
    {
        printf(USAGE_ERR_MSG);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
