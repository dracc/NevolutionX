#ifndef NEVOLUTIONX_XBELAUNCHER_H
#define NEVOLUTIONX_XBELAUNCHER_H

#include <string>

// Launches an XBE (or the dashboard).
class XBELauncher {
public:
  static void exitToDashboard();
  static void launch(std::string const& xbePath);

private:
  // TODO(#113): Add support for a pre-launch image.
  static void showLaunchImage();
};

#endif // NEVOLUTIONX_XBELAUNCHER_H
