#include "timeMenu.hpp"

#include <utility>
#include <vector>
#include <xboxkrnl/xboxkrnl.h>


TimeItem::TimeItem(MenuNode *parent, std::string const& label) :
  MenuItem(parent, label) {

}

void TimeItem::execute(Menu *menu) {
}

TimeMenu::TimeMenu(MenuNode *parent, std::string const& label) :
  MenuNode(parent, label) {
  timeZones.emplace_back("Samoa", (int32_t)0x00000294, "NT", "NT",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Hawaii", (int32_t)0x00000258, "HST", "HST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Alaska", (int32_t)0x0000021C, "YST", "YDT",
                         0x0200050A, 0x02000104, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Pacific", (int32_t)0x000001E0, "PST", "PDT",
                         0x0200050A, 0x02000104, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Mountain", (int32_t)0x000001A4, "MST", "MST",
                         0x0200050A, 0x02000104, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Arizona", (int32_t)0x000001A4, "MST", "MST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Saskatchewan", (int32_t)0x00000168, "CCST", "CCST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("MexicoCity", (int32_t)0x00000168, "MST", "MDT",
                         0x0200050A, 0x02000104, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Central", (int32_t)0x00000168, "CST", "CDT",
                         0x0200050A, 0x02000104, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("CentralAmerica", (int32_t)0x00000168, "CAST", "CAST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Indiana", (int32_t)0x0000012C, "EST", "EST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Eastern", (int32_t)0x0000012C, "EST", "EDT",
                         0x0200050A, 0x02000104, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Bogota", (int32_t)0x0000012C, "SPST", "SPST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Santiago", (int32_t)0x000000F0, "PSST", "PSDT",
                         0x00060203, 0x0006020A, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Caracas", (int32_t)0x000000F0, "SWST", "SWST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Atlantic", (int32_t)0x00F0FB57, "AS", "TAD",
                         0x050A0000, 0x01040200, (int32_t)0x00000000, (int32_t)0xFFC40000);
  timeZones.emplace_back("Greenland", (int32_t)0x000000B4, "GST", "GDT",
                         0x0200050A, 0x02000104, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("BuenosAires", (int32_t)0x000000B4, "SEST", "SEST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Brasilia", (int32_t)0x000000B4, "ESST", "ESDT",
                         0x02000202, 0x0200030A, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("MidAtlantic", (int32_t)0x00000078, "MAST", "MADT",
                         0x02000509, 0x02000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("CapeVerdeIslands", (int32_t)0x0000003C, "WAT", "WAT",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Azores", (int32_t)0x0000003C, "AST", "ADT",
                         0x0300050A, 0x02000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Casablanca", (int32_t)0x00000000, "GST", "GST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("London", (int32_t)0x00000000, "GMT", "BST",
                         0x0200050A, 0x01000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Belgrade", (int32_t)0xFFFFFFC4, "CEST", "CEDT",
                         0x0300050A, 0x02000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Berlin", (int32_t)0xFFFFFFC4, "WEST", "WEDT",
                         0x0300050A, 0x02000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Paris", (int32_t)0xFFFFFFC4, "RST", "RDT",
                         0x0300050A, 0x02000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Sarajevo", (int32_t)0xFFFFFFC4, "SCST", "SCDT",
                         0x0300050A, 0x02000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("WestCentralAfrica", (int32_t)0xFFFFFFC4, "WAST", "WAST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Athens", (int32_t)0xFFFFFF88, "GTST", "GTDT",
                         0x0300050A, 0x02000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Bucharest", (int32_t)0xFFFFFF88, "EEST", "EEDT",
                         0x01000509, 0x00000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Cairo", (int32_t)0xFFFFFF88, "EST", "EDT",
                         0x02030509, 0x02050105, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Helsinki", (int32_t)0xFFFFFF88, "FLST", "FLDT",
                         0x0400050A, 0x03000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Jerusalem", (int32_t)0xFFFFFF88, "JST", "JST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Pretoria", (int32_t)0xFFFFFF88, "SAST", "SAST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Baghdad", (int32_t)0xFFFFFF4C, "AST", "ADT",
                         0x0400010A, 0x03000104, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Kuwait", (int32_t)0xFFFFFF4C, "AST", "AST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Moscow", (int32_t)0xFFFFFF4C, "RST", "RDT",
                         0x0300050A, 0x02000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Nairobi", (int32_t)0xFFFFFF4C, "EAST", "EAST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("AbuDhabi", (int32_t)0xFFFFFF10, "AST", "AST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Baku", (int32_t)0xFFFFFF10, "CST", "CDT",
                         0x0300050A, 0x02000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Ekaterinburg", (int32_t)0xFFFFFED4, "EST", "EDT",
                         0x0300050A, 0x02000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Islamabad", (int32_t)0xFFFFFED4, "WAST", "WAST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Almaty", (int32_t)0xFFFFFE98, "NCST", "NCDT",
                         0x0300050A, 0x02000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Dhaka", (int32_t)0xFFFFFE98, "CAST", "CAST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Srilanka", (int32_t)0xFFFFFE98, "SRST", "SRST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Bangkok", (int32_t)0xFFFFFE5C, "SAST", "SAST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Krasnoyarsk", (int32_t)0xFFFFFE5C, "NAST", "NADT",
                         0x0300050A, 0x02000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Beijing", (int32_t)0xFFFFFE20, "CST", "CST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Irkutsk", (int32_t)0xFFFFFE20, "NEST", "NEDT",
                         0x0300050A, 0x02000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Perth", (int32_t)0xFFFFFE20, "AWST", "AWST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Singapore", (int32_t)0xFFFFFE20, "MPST", "MPST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Taipei", (int32_t)0xFFFFFE20, "TST", "TST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Seoul", (int32_t)0xFFFFFDE4, "KST", "KST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Tokyo", (int32_t)0xFFFFFDE4, "TST", "TST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Yakutsk", (int32_t)0xFFFFFDE4, "YST", "YDT",
                         0x0300050A, 0x02000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Brisbane", (int32_t)0xFFFFFDA8, "AEST", "AEST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Guam", (int32_t)0xFFFFFDA8, "WPST", "WPST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Hobart", (int32_t)0xFFFFFDA8, "TST", "TDT",
                         0x02000503, 0x0200010A, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Sydney", (int32_t)0xFFFFFDA8, "AEST", "AEDT",
                         0x02000503, 0x0200050A, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Vladivostok", (int32_t)0xFFFFFDA8, "VST", "VDT",
                         0x0300050A, 0x02000503, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("SolomonIslands", (int32_t)0xFFFFFD6C, "CPST", "CPST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Auckland", (int32_t)0xFFFFFD30, "NZST", "NZDT",
                         0x02000303, 0x0200010A, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("FijiIslands", (int32_t)0xFFFFFD30, "FST", "FST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Nukualofa", (int32_t)0xFFFFFCF4, "TST", "TST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Kiribati", (int32_t)0xFFFFFCB8, "KST", "KST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Tehran", (int32_t)0xFFFFFF2E, "IST", "IDT",
                         0x02020409, 0x02000103, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Kabul", (int32_t)0xFFFFFEF2, "AST", "AST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("NewDelhi", (int32_t)0xFFFFFEB6, "IST", "IST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Kathmandu", (int32_t)0xFFFFFEA7, "NST", "NST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Yangon", (int32_t)0xFFFFFE7A, "MST", "MST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  timeZones.emplace_back("Adelaide", (int32_t)0xFFFFFDC6, "ACST", "ACDT",
                         0x02000503, 0x0200050A, (int32_t)0x00000000, (int32_t)0xFFFFFFC4);
  timeZones.emplace_back("Darwin", (int32_t)0xFFFFFDC6, "ACST", "ACST",
                         0x00000000, 0x00000000, (int32_t)0x00000000, (int32_t)0x00000000);
  for (size_t i = 0; i < timeZones.size(); ++i) {
    std::shared_ptr<TimeItem> newNode = std::make_shared<TimeItem>(this, timeZones[i].name);
    addNode(newNode);
  }
}

void TimeMenu::execute(Menu *menu) {

  if (menu->getCurrentMenu() != this) {
    menu->setCurrentMenu(this);
  }
  else {
    if (childNodes.size() > selected) {
      unsigned char* stdname[5];
      unsigned char* dstname[5];
      std::memcpy(stdname, timeZones[selected].stdname.c_str(), 4);
      std::memcpy(dstname, timeZones[selected].dstname.c_str(), 4);
      ExSaveNonVolatileSetting(0x0, 4, const_cast<int*>(&timeZones[selected].bias), 4);
      ExSaveNonVolatileSetting(0x1, 3, &stdname, 4);
      ExSaveNonVolatileSetting(0x2, 3, const_cast<uint32_t*>(&timeZones[selected].stdstart), 4);
      ExSaveNonVolatileSetting(0x3, 4, const_cast<int*>(&timeZones[selected].stdbias), 4);
      ExSaveNonVolatileSetting(0x4, 3, &dstname, 4);
      ExSaveNonVolatileSetting(0x5, 3, const_cast<uint32_t*>(&timeZones[selected].dststart), 4);
      ExSaveNonVolatileSetting(0x6, 4, const_cast<int*>(&timeZones[selected].dstbias), 4);
      menu->setCurrentMenu(this->getParent());
    }
  }
}
