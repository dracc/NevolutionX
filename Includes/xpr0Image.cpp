#include "xpr0Image.h"
#include "3rdparty/s3tc-dxt-decompression/s3tc.h"
#include "infoLog.h"

static const uint32_t XPR0_MAGIC = 0x30525058;

bool XPR0Image::parse(const std::vector<uint8_t>& buffer) {
  auto& header = *reinterpret_cast<XPRHeader const*>(buffer.data());
  if (header.magic != XPR0_MAGIC) {
    InfoLog::outputLine(InfoLog::ERROR, "Unexpected magic bytes %X in XPR0", header.magic);
    return false;
  }

  static const uint32_t FORMAT_MASK = 0x0000FF00;
  format = header.resourceInfo.format & FORMAT_MASK;

  static const uint32_t FORMAT_DXT1 = 0x00000C00;
  // TODO: Investigate whether formats other than DXT1 are ever used.
  if (format != FORMAT_DXT1) {
    InfoLog::outputLine(InfoLog::ERROR, "Unexpected format %X (!=DXT1) in XPR0",
                        header.resourceInfo.format);
    return false;
  }

  uint32_t dataSize = header.totalSize - header.headerSize;
  if (dataSize > buffer.size()) {
    InfoLog::outputLine(InfoLog::ERROR, "Buffer size too small (%u < %u) in XPR0",
                        buffer.size(), dataSize);
  }

  static const uint32_t UV_SIZE_MASK = 0x0FF00000;
  static const uint32_t U_SHIFT = 20;
  static const uint32_t V_SHIFT = 24;
  const uint32_t sizeInfo = header.resourceInfo.format & UV_SIZE_MASK;
  width = 1 << ((sizeInfo >> U_SHIFT) & 0x0F);
  height = 1 << ((sizeInfo >> V_SHIFT) & 0x0F);

  auto imageDataStart = buffer.cbegin() + static_cast<int>(header.headerSize);
  imageData = std::vector<uint8_t>(imageDataStart, buffer.cend());

  return true;
}

bool XPR0Image::decompress(std::vector<uint8_t>& output) const {
  output.resize(width * height * 4);
  return decompress(output.data());
}

bool XPR0Image::decompress(uint8_t* output) const {
  BlockDecompressImageDXT1(width, height, imageData.data(), (unsigned long*)output);
  return true;
}
