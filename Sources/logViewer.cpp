#include "logViewer.hpp"
#include <iterator>
#include "infoLog.hpp"
#include "subAppRouter.hpp"

// Dummy index indicating that the log should always show the most recent
// message at the bottom of the screen.
#define LOG_INDEX_PINNED_BOTTOM -1

logViewer::logViewer(Renderer& r) : renderer(r) {
  auto w = static_cast<float>(renderer.getWidth());
  auto h = static_cast<float>(renderer.getHeight());
  renderArea.x = w * 0.1f;
  renderArea.y = h * 0.05f;
  renderArea.w = w - (renderArea.x * 2.0f);
  renderArea.h = h - (renderArea.y * 2.0f);

  endIndex = LOG_INDEX_PINNED_BOTTOM;
}


void logViewer::render(Font& font) {
  renderer.setDrawColor(0, 0, 0, 0x70);
  renderer.fillRectangle(renderArea);

  float bottom = renderArea.y + renderArea.h;
  {
    std::lock_guard<std::mutex> lock(InfoLog::getLogMutex());
    const std::list<std::string>& log = InfoLog::getLog();

    auto it = log.rbegin();
    auto itEnd = log.rend();

    if (endIndex != LOG_INDEX_PINNED_BOTTOM) {
      int lastIndex = static_cast<int>(log.size()) - 1;
      for (int i = 0; it != itEnd && i < (lastIndex - endIndex); ++i, ++it) {
        // Skip offscreen items.
      }
    }

    for (; it != itEnd; ++it) {
      const std::string& line = *it;

      auto lineHeight = font.getColumnHeight(line, renderArea.w);
      bottom -= lineHeight;

      if (bottom < renderArea.y) {
        // TODO: Support partial lines via clipping.
        break;
      }

      auto coordinates = std::make_pair(renderArea.x, bottom);
      font.drawColumn(line, coordinates, renderArea.w);
    }
  }
}

void logViewer::scroll(int delta) {
  int logSize = InfoLog::getLogSize();
  int lastIndex = logSize - 1;

  if (endIndex == LOG_INDEX_PINNED_BOTTOM) {
    if (delta > 0) {
      return;
    }
    endIndex = lastIndex;
  }

  endIndex += delta;
  if (endIndex < 0) {
    endIndex = 0;
  } else if (endIndex > lastIndex) {
    // An attempt to move past the bottom of the list will trigger automatic
    // scrolling if new messages are received.
    endIndex = LOG_INDEX_PINNED_BOTTOM;
  }
}

void logViewer::quit() {
  SubAppRouter::getInstance()->pop();
}
