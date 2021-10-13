#ifndef NEVOLUTIONX_XPR0IMAGE_H
#define NEVOLUTIONX_XPR0IMAGE_H

#include <cstdint>
#include <vector>

// Encapsulates information about an XPR0 image.
class XPR0Image {
public:
  struct ResourceInfo {
    uint32_t common;
    uint32_t data;
    uint32_t lock;
    uint32_t format;
    uint32_t size;
  };

  struct XPRHeader {
    uint32_t magic;
    uint32_t totalSize;
    uint32_t headerSize;
    ResourceInfo resourceInfo;
    uint32_t endOfHeader; // Should always == 0xFFFFFFFF
  };

  // Populates this XPR0Image from the given data buffer.
  bool parse(std::vector<uint8_t> const& buffer);

  // Copies 32bpp decompressed image data into the given `output` buffer.
  //
  // Returns true if the operation succeded, false if there was an error.
  bool decompress(std::vector<uint8_t>& output) const;

  bool decompress(uint8_t* output) const;

  void clear() {
    width = height = format = 0;
    imageData.clear();
  }

  uint32_t width{ 0 };
  uint32_t height{ 0 };
  uint32_t format;
  std::vector<uint8_t> imageData;
};

#endif // NEVOLUTIONX_XPR0IMAGE_H
