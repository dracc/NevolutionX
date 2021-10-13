#include "xbeInfo.h"
#include "infoLog.h"

XBEInfo::Icon XBEInfo::loadSaveGameIcon() const {
  Icon ret;
  if (saveGameXPROffset <= 0 || saveGameXPRSize <= 0) {
    return ret;
  }

  XPR0Image compressedImage;
  if (!loadCompressedSaveGameIcon(compressedImage)) {
    InfoLog::outputLine(InfoLog::WARNING, "Failed to load save game icon from %s",
                        path.c_str());
    return ret;
  }

  if (!compressedImage.decompress(ret.imageData)) {
    InfoLog::outputLine(InfoLog::WARNING, "Failed to decompress save game icon from %s",
                        path.c_str());
    return ret;
  }

  ret.width = compressedImage.width;
  ret.height = compressedImage.height;

  return ret;
}

bool XBEInfo::loadCompressedSaveGameIcon(XPR0Image& image) const {
  image.clear();
  FILE* xbeFile = fopen(path.c_str(), "rb");
  if (!xbeFile) {
    return false;
  }

  fseek(xbeFile, saveGameXPROffset, SEEK_SET);
  std::vector<uint8_t> buffer(saveGameXPRSize);
  size_t bytesRead = fread(buffer.data(), 1, saveGameXPRSize, xbeFile);
  fclose(xbeFile);

  if (bytesRead != saveGameXPRSize) {
    InfoLog::outputLine(InfoLog::WARNING, "Failed to read save game image from %s",
                        path.c_str());
    return false;
  }

  return image.parse(buffer);
}
