#include "xbeLauncher.h"
#include <hal/video.h>
#include <hal/xbox.h>

void XBELauncher::exitToDashboard() { showLaunchImage();
  // TODO: Switch to XLaunchXBE(nullptr) if XboxDev/nxdk#501 is merged.
  exit(0);
}

void XBELauncher::launch(std::string const &xbePath)
{
  showLaunchImage();
  XLaunchXBE(const_cast<char*>(xbePath.c_str()));
}

void XBELauncher::showLaunchImage() {
  VIDEO_MODE mode = XVideoGetMode();

  // TODO(XboxDev/nxdk#507): Display launch image instead when framebuffer can be persisted.
  unsigned char* fb = XVideoGetFB();
  memset(fb, 0, mode.width * mode.height * (mode.bpp >> 3));
  XVideoFlushFB();
  XVideoSetVideoEnable(FALSE);
}
