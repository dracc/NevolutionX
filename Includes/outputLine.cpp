#include "outputLine.h"

#include <cstdarg>
#include <cstdio>

#ifdef NXDK
#include <windows.h>
#include <hal/debug.h>
#else
#include <SDL.h>
#endif

#include "infoLog.h"

void outputLine(const char* format, ...) {
  char buffer[4096];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);

  if (InfoLog::isOutputCaptured()) {
    InfoLog::outputLine(buffer);
  } else {
#ifdef NXDK
    debugPrint("%s", buffer);
    OutputDebugStringA(buffer);
#else
    printf("%s", buffer);
#endif
  }

  va_end(args);
}
