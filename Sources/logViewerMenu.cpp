#include "logViewerMenu.hpp"
#include "logViewer.hpp"
#include "subAppRouter.hpp"

logViewerMenu::logViewerMenu(MenuNode* parent, const std::string& label) :
    MenuItem(parent, label) {
}

void logViewerMenu::execute(Menu* m) {
  SubAppRouter::getInstance()->push(std::make_shared<logViewer>(m->getRenderer()));
}
