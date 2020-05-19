#ifndef FINDXBE_H
#define FINDXBE_H

#include <vector>
#include <string>
#include "../menu/xbeMenuItem.h"

struct xbeFinderArg {
  std::vector<xbeMenuItem> *list;
  std::string path;
};

int findXBE(void* list);

#endif
