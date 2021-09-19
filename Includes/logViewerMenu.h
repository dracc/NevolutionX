#ifndef NEVOLUTIONX_INCLUDES_LOGVIEWERMENU_H_
#define NEVOLUTIONX_INCLUDES_LOGVIEWERMENU_H_

#include "menu.hpp"

class logViewerMenu : public MenuItem {
public:
  logViewerMenu(MenuNode *parent, std::string const& label);

  void execute(Menu *) override;
};

#endif //NEVOLUTIONX_INCLUDES_LOGVIEWERMENU_H_
