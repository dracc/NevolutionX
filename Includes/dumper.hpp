#ifndef __DUMPER_HPP
#define __DUMPER_HPP

#include <string>
#include "menu.hpp"

class Dumper : public MenuItem {
public:
  Dumper(std::string const& label);
  void execute(Menu*);
};

#endif
