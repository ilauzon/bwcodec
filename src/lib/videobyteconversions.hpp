#include "structs.h"
#include <vector>

namespace videobyteconversions {

std::vector<std::byte> toBytes(Video &video);
Video toVideo(std::vector<std::byte> &bytes);

} // namespace videobyteconversions
