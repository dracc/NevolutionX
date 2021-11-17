#ifndef MENU_HPP
#define MENU_HPP

#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "config.hpp"
#include "font.hpp"
#include "subApp.hpp"
#include "xbeScanner.hpp"

class MenuNode;
class Menu;

class MenuItem {
public:
  explicit MenuItem(std::string const& label);
  MenuItem(MenuNode* parent, std::string const& label);
  virtual ~MenuItem();
  virtual void execute(Menu*) = 0;
  MenuNode* getParent() const;
  virtual std::string_view getLabel() const;

  void setParent(MenuNode* parent);

protected:
  MenuNode* parentNode;
  const std::string label;
};

class MenuNode : public MenuItem {
public:
  explicit MenuNode(std::string const& label);
  MenuNode(MenuNode* parent, std::string const& label);
  ~MenuNode() override;
  void execute(Menu*) override;
  void setSelected(size_t id);
  size_t getSelected();
  virtual std::vector<std::shared_ptr<MenuItem>>* getChildNodes();
  void addNode(std::shared_ptr<MenuItem> node);

  void up();
  void down();
  void pageUp(int pageSize);
  void pageDown(int pageSize);

  virtual void superscrollUp() {}
  virtual void superscrollDown() {}

protected:
  void moveSelection(int delta, bool allowWrap = true);

  size_t selected = 0;
  std::vector<std::shared_ptr<MenuItem>> childNodes;
};

// A non-interactive menu entry that simply displays a label.
class MenuNonInteractive : public MenuNode {
public:
  MenuNonInteractive(MenuNode* parent, std::string const& label);
  void execute(Menu*) override;
};

class MenuXbe : public MenuNode {
public:
  MenuXbe(MenuNode* parent, std::string const& label, std::string const& paths);
  ~MenuXbe() override;
  void execute(Menu* menu) override;

  std::vector<std::shared_ptr<MenuItem>>* getChildNodes() override;

  void superscrollUp() override { superscroll(true); }
  void superscrollDown() override { superscroll(false); }

private:
  void superscroll(bool moveToPrevious);
  void updateScanningLabel();
  void onScanCompleted(bool succeeded,
                       std::list<XBEScanner::XBEInfo> const& items,
                       long long duration);
  void createChildren();

  std::mutex childNodesLock;
  std::list<std::string> remainingScanPaths;
  std::vector<XBEScanner::XBEInfo> discoveredItems;

  // Map of first letter to index of the first child in childNodes whose label starts with
  // that letter.
  std::map<int, size_t> superscrollIndex;
};

class MenuLaunch : public MenuItem {
public:
  MenuLaunch(std::string const& label, std::string const& path);
  ~MenuLaunch() override;
  void execute(Menu*) override;

protected:
  std::string path;
};

class MenuExec : public MenuItem {
public:
  MenuExec(std::string const& label, void execute(Menu*));
  ~MenuExec() override;
  void execute(Menu*) override;
  void (*action)(Menu*);
};

class Menu : public SubApp {
public:
  Menu(const Config& config, Renderer& renderer);
  void render(Font& font) override;
  MenuNode* getCurrentMenu();
  void setCurrentMenu(MenuNode*);

  inline Renderer& getRenderer() { return renderer; }

  void onUpPressed() override;
  void onDownPressed() override;
  void onLeftPressed() override { pageUp(); }
  void onRightPressed() override { pageDown(); }
  void onAPressed() override { execute(); }
  void onStartPressed() override { execute(); }
  void onBackPressed() override { back(); }
  void onBPressed() override { back(); }

  void onLeftStickDigitalUpPressed() override { onUpPressed(); }
  void onLeftStickDigitalDownPressed() override { onDownPressed(); }
  void onLeftStickDigitalLeftPressed() override { onLeftPressed(); }
  void onLeftStickDigitalRightPressed() override { onRightPressed(); }

  void onRightStickDigitalUpPressed() override { superscrollUp(); }
  void onRightStickDigitalDownPressed() override { superscrollDown(); }

  void pageUp();
  void pageDown();
  void superscrollUp();
  void superscrollDown();

  void back();
  void execute();

protected:
  Renderer& renderer;
  MenuNode rootNode;
  MenuNode* currentMenu;
  int menuHeight;
  int startHeight;
  size_t offsetForDraw = 0;
  size_t itemsToShow = 16;
  size_t lowerHalf = 9;
  size_t upperHalf = 7;
};

#endif
