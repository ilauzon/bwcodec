#include <cstdlib>
#include <fstream>
#include <string>

#define USAGE_ERR_MSG "Usage: bwcodec <encode|decode> <frames_directory> <video_filename>\n"

void decode(std::string videoFilename, std::string framesDirectory) 
{
    std::ifstream videoFileStream(videoFilename, std::ios::binary);
    videoFileStream.close();
}

void encode(std::string framesDirectory, std::string videoFilename) 
{
    std::ofstream videoFileStream(videoFilename, std::ios::binary);
    videoFileStream.close();
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
