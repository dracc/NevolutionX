#include "theme.hpp"
#include <fstream>
#include <utility>

#define THEME_JSON_FILE_NAME "theme.json"

Theme::Theme(std::string themeDirectory) : rootPath(std::move(themeDirectory)) {
  if (rootPath.back() != '\\') {
    rootPath += "\\";
  }
  load();
}

void Theme::load() {
  std::string themeFilePath = rootPath + THEME_JSON_FILE_NAME;

  std::ifstream themeFile(themeFilePath);
  nlohmann::json json;
  // FIXME: Once nxdk supports C++ Exceptions, this needs to be put in a try-catch block!
  themeFile >> json;
  from_json(json, *this);

  themeFile.close();
}

void to_json(nlohmann::json& j, Theme::MenuTheme const& o) {
  char fontColor[16] = { 0 };
  snprintf(fontColor, 15, "%X", o.fontColor);

  j = nlohmann::json{ { "font", nlohmann::json(o.font) },
                      { "font_color", nlohmann::json(fontColor) } };
}

void from_json(nlohmann::json const& j, Theme::MenuTheme& o) {
  if (j.contains("font")) {
    o.font = j["font"];
  }
  if (j.contains("font_color")) {
    std::string const& colorString = j["font_color"];
    sscanf(colorString.c_str(), "%X", &o.fontColor);
  }
}

void to_json(nlohmann::json& j, Theme::ImageSet const& o) {
  j = nlohmann::json{ { "480", nlohmann::json(o.image480p) },
                      { "720", nlohmann::json(o.image720p) } };
}

void from_json(nlohmann::json const& j, Theme::ImageSet& o) {
  if (j.contains("480")) {
    o.image480p = j["480"];
  }
  if (j.contains("720")) {
    o.image720p = j["720"];
  }
}

void to_json(nlohmann::json& j, Theme const& o) {
  j = nlohmann::json{ { "title", o.getTitle() },
                      { "menu", nlohmann::json(o.getMenu()) },
                      { "background", nlohmann::json(o.getBackground()) } };
}

void from_json(nlohmann::json const& j, Theme& o) {
  if (j.contains("title")) {
    o.setTitle(j["title"]);
  }
  if (j.contains("menu")) {
    o.setMenu(j["menu"].get<Theme::MenuTheme>());
  }
  if (j.contains("background")) {
    o.setBackground(j["background"].get<Theme::ImageSet>());
  }
}
