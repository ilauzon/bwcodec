#include "structs.h"
#include <vector>

namespace videobyteconversions {

std::vector<std::byte> toBytes(const Video &video);
Video toVideo(const std::vector<std::byte> &bytes);

} // namespace videobyteconversions
