#include "strdup.h"

#include <string.h>
#include <stdlib.h>

char* strdup(const char* src) {
  char* ret = static_cast<char*>(malloc(strlen(src) * sizeof(char) + 1));
  strcpy(ret, src);
  return ret;
}
