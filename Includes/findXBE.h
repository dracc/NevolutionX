#ifndef FINDXBE_H
#define FINDXBE_H

#include <vector>
#include <string>
#include "xbeMenuItem.h"

#ifdef NXDK
#include "folderparse.h"
#include "xbe.h"
#include <hal/xbox.h>
#include <stdio.h>
#endif

struct xbeFinderArg {
  std::vector<xbeMenuItem> *list;
  std::string path;
};

int findXBE(void* list);

#endif
