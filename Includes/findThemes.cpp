//
// Created by kosmas on 28/7/21.
//

#include "findThemes.h"
#include "outputLine.h"
#ifdef NXDK
#include <windows.h>
#endif

void findThemes(std::string const& path, MenuThemes *list) {
  std::string workPath = path;
  if (workPath.back() != '\\') {
    workPath += '\\';
  }
  std::string searchMask = workPath + "*";

  WIN32_FIND_DATA findFileData;
  HANDLE hFind;
  std::string tmp;

  hFind = FindFirstFile(searchMask.c_str(), &findFileData);
  if (hFind == INVALID_HANDLE_VALUE) {
    outputLine("FindFirstHandle() failed!\n");
  }

  do {
    if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      std::string type = path.substr(path.find(".") + 1);
        if (type == "nevoxtheme") {
          tmp = path + findFileData.cFileName;
          list->addNode(std::make_shared<MenuLaunch>(findFileData.cFileName, tmp));
        }
   }
  } while (FindNextFile(hFind, &findFileData) != 0);
}
