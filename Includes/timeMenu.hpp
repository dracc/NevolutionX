#ifndef TIMEMENU_HPP
#define TIMEMENU_HPP

#include <memory>
#include <string>
#include "menu.hpp"

class tzData {
public:
  const std::string name;
  int32_t bias;
  const std::string stdname;
  const std::string dstname;
  uint32_t stdstart;
  uint32_t dststart;
  int32_t stdbias;
  int32_t dstbias;
  tzData(std::string const& name,
         int32_t bias,
         std::string const& stdname,
         std::string const& dstname,
         uint32_t stdstart,
         uint32_t dststart,
         int32_t stdbias,
         int32_t dstbias) :
      name(name), bias(bias), stdname(stdname), dstname(dstname), stdstart(stdstart),
      dststart(dststart), stdbias(stdbias), dstbias(dstbias) {};
};

class TimeItem : public MenuItem {
public:
  TimeItem(MenuNode* parent, std::string const& label);

  void execute(Menu* menu);
};

class TimeMenu : public MenuNode {
public:
  TimeMenu(MenuNode* parent, std::string const& label);
  void execute(Menu* menu);

private:
  std::vector<tzData> timeZones;
};

#endif
