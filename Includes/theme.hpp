#ifndef NEVOLUTIONX_THEME_H
#define NEVOLUTIONX_THEME_H

#include <string>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
#include "../3rdparty/json.hpp"
#pragma clang diagnostic pop


class Theme {
public:
  struct MenuTheme {
    std::string font;
    // TODO: Actually support this in Font.
    unsigned int fontColor;
  };

  struct ImageSet {
    std::string image480p;
    std::string image720p;
  };

  explicit Theme(std::string themeDirectory);

  void setTitle(std::string const& val) { title = val; }
  std::string const& getTitle() const { return title; }

  void setBackground(ImageSet const& val) { background = val; }
  ImageSet const& getBackground() const { return background; }

  void setMenu(MenuTheme const& val) { menu = val; }
  MenuTheme const& getMenu() const { return menu; }

  std::string getAbsolutePath(std::string const& subpath) const {
    return rootPath + subpath;
  }

private:
  void load();

  std::string rootPath;
  std::string title{ "??MISSING??" };
  ImageSet background;
  MenuTheme menu;
};

void to_json(nlohmann::json& j, Theme const& o);
void from_json(nlohmann::json const& j, Theme& o);

void to_json(nlohmann::json& j, Theme::MenuTheme const& o);
void from_json(nlohmann::json const& j, Theme::MenuTheme& o);

void to_json(nlohmann::json& j, Theme::ImageSet const& o);
void from_json(nlohmann::json const& j, Theme::ImageSet& o);

#endif // NEVOLUTIONX_THEME_H
