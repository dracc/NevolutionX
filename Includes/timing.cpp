#include "timing.h"

long long millisSince(const std::chrono::steady_clock::time_point& ref) {
  auto now = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now - ref).count();
}
