# bwcodec

An experimental lossless video codec for sprite sheets consisting of pure black or pure white pixels (for now).

Goal: Surpass the compression ratio for H.265 with conventional sprite sheets (noise will likely be encoded to a smaller size by H.265 due to lossy compression).

## How to build
Dependencies:
- CMake >= 3.23
- OpenCV >= 4.11 development headers

### Linux

Build the project using the following commands:
```shell
cmake -B build
cmake --build build
```
The built executable is `build/src/bwcodec`.

## Milestones
Below is a minimal list of milestones for this project.

- [ ] Support for coding with a series of PNGs
- [ ] Inter-frame coding
- [ ] Intra-frame coding (refer to PNG coding at this milestone)
- [ ] Playback as a resizable video stream in a window (could use a web browser as an example)
- [ ] Support for encoding using a video source in a different codec
- [ ] Multithreading support
