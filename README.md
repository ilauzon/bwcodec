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

## Benchmarks

The H.265 MP4s are generated with ffmpeg using a CRF of 28.
```shell 
ffmpeg -framerate 30 -i frame%d.jpg -c:v libx265 -crf 28 output.mp4
```

| Video                     | PNG Series | H.265 MP4 | bwcodec (Milestone 1)    |
|---------------------------|------------|-----------|--------------------------|
| Bad Apple, 480x360        | 16.1 MiB   | 12.4 MiB  | 135.4 MiB                |

## Milestones
Below is a minimal list of milestones for this project.

- [x] 1. Support for coding with a series of PNGs
- [ ] 2. Inter-frame coding
- [ ] 3. Intra-frame coding (refer to PNG coding at this milestone)
- [ ] 4. Playback as a resizable video stream in a window (could use a web browser as an example)
- [ ] 5. Support for encoding using a video source in a different codec
- [ ] 6. Multithreading support
