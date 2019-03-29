#include "outputLine.h"

void outputLine(char* format, ...) {
  char buffer[4096];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
#ifdef NXDK
  debugPrint("%s", buffer);
#else
  SDL_Log("%s", buffer);
#endif
  va_end(args);
}
