#ifndef FINDXBE_H
#define FINDXBE_H

#include "vector.hpp"
#include "xbeMenuItem.h"

#ifdef NXDK
#include "folderparse.h"
#include <hal/xbox.h>
#include <stdio.h>
#endif

struct xbeFinderArg {
  vector<xbeMenuItem> *list;
  char *path;
};

int findXBE(void* list);

#endif
