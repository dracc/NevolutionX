#include "logViewerMenu.h"

#include "logViewer.h"
#include "subAppRouter.h"

logViewerMenu::logViewerMenu(MenuNode *parent, const std::string &label) : MenuItem(parent, label) {
}

void logViewerMenu::execute(Menu *m) {
  SubAppRouter::getInstance()->push(std::make_shared<logViewer>(m->getRenderer()));
}
