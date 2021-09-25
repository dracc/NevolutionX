#ifndef NEVOLUTIONX_XBEINFO_H
#define NEVOLUTIONX_XBEINFO_H

#include <string>
#include <vector>
#include "xpr0Image.h"

class XBEInfo {
public:
  // TODO: See if the DXT1 compressed image can be used directly by the hardware instead.
  struct Icon {
    // imageData is always 32-bit color.
    std::vector<unsigned char> imageData;
    uint32_t width{ 0 };
    uint32_t height{ 0 };
  };

  XBEInfo(std::string xbeTitle, std::string xbePath, long xprOffset, size_t xprSize) :
      title(std::move(xbeTitle)), path(std::move(xbePath)), saveGameXPROffset(xprOffset),
      saveGameXPRSize(xprSize) {}

  Icon loadSaveGameIcon() const;
  bool loadCompressedSaveGameIcon(XPR0Image& image) const;

  std::string title;
  std::string path;

private:
  long saveGameXPROffset{ 0 };
  size_t saveGameXPRSize{ 0 };
};

#endif // NEVOLUTIONX_XBEINFO_H
