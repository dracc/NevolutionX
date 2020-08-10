#ifndef FINDXBE_H
#define FINDXBE_H

#include <vector>
#include <string>
#include "xbeMenuItem.h"
#include "menu.hpp"

struct xbeFinderArg {
  std::vector<xbeMenuItem> *list;
  std::string path;
};

int findXBE(std::string const& path, MenuXbe *list);

#endif
