#ifndef FINDXBE_H
#define FINDXBE_H

#include "vector.hpp"
#include "gameMenuItem.h"

#ifdef NXDK
#include "folderparse.h"
#include <hal/xbox.h>
#include <stdio.h>
#endif

struct xbeFinderArg {
  vector<gameMenuItem> *list;
  char *path;
};

int findXBE(void* list);

#endif
