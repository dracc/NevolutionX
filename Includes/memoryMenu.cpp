#include "memoryMenu.hpp"

#include <algorithm>
#include <fstream>
#include <vector>
#include <windows.h>

static const std::string gametitlemetaname = "TitleMeta.xbx";
static const std::string gamesavemetaname = "SaveMeta.xbx";

static void recursiveDelFolder(std::string const& path) {
  std::string searchmask = path + "\\*";
  WIN32_FIND_DATAA fData;
  HANDLE fHandle = FindFirstFileA(searchmask.c_str(), &fData);
  if (fHandle == INVALID_HANDLE_VALUE) {
     return;
  }

  do {
    if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      recursiveDelFolder(path + "\\" + fData.cFileName);
    } else {
      DeleteFileA((path + "\\" + fData.cFileName).c_str());
    }
  } while (FindNextFile(fHandle, &fData) != 0);
  FindClose(fHandle);
  RemoveDirectoryA(path.c_str());
}

static
std::string extractName(std::string const& path, std::string const& file) {
  std::ifstream f(path + "\\" + file, std::ios::binary | std::ios::ate);
  if (!f.is_open()) {
    return "";
  }
  std::string buf;
  size_t fileSize = f.tellg();
  f.seekg(0, std::ios::beg);

  std::vector<char> buffer(fileSize);
  f.read(buffer.data(), fileSize);
  if (buffer.size() > 2) {
    // The files we're expecting starts with the magic value 0xFFFE
    // If that's not found, return an empty string
    if (buffer[0] == (char)0xFF && buffer[1] == (char)0xFE) {
      for (auto i = buffer.begin() + 2; i != buffer.end(); ++i) {
        if (*i == '\r') {
          break;
        }
        if (*i != 0x00) {
          buf.append(1, *i);
        }
      }
      size_t equalMark = buf.find("=");
      if (equalMark) {
        buf = buf.substr(equalMark + 1);
      }
    }
  }
  return buf;
}

static
std::vector<std::string> findSubFolders(std::string const& path) {
  std::vector<std::string> ret;
#ifdef NXDK
  std::string searchmask = path + "\\*";
  WIN32_FIND_DATAA fData;
  HANDLE fHandle = FindFirstFileA(searchmask.c_str(), &fData);
  if (fHandle == INVALID_HANDLE_VALUE) {
    ret.push_back("<Empty>");
    return ret;
  }

  do {
    if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      ret.push_back(path + "\\" + fData.cFileName);
    }
  } while (FindNextFile(fHandle, &fData) != 0);
  FindClose(fHandle);
#else
  for (int i = 0; i < 7; ++i) {
    ret.push_back("");
  }
#endif
  return ret;
}

/*******************************************************************************
 *                             gameSaveData
 ******************************************************************************/
gameSaveData::gameSaveData(MenuNode *parent, std::string const& label, std::string const& path) :
  MenuItem(parent, label), path(path) {
  size = 192;
}

// std::string_view gameSaveData::getLabel() const {
//   return label + " (" + std::to_string(size) + ")";
// }

void gameSaveData::execute(Menu *) {
#ifdef NXDK
  /* FIXME: Output some kind of warning and require confirmation */
  recursiveDelFolder(path);
  getParent()->getChildNodes()->erase(getParent()->getChildNodes()->begin() +
                                      getParent()->getSelected());
#endif
}

/*******************************************************************************
 *                               gameSave
 ******************************************************************************/
gameSave::gameSave(MenuNode *parent,
                   std::string const& label,
                   std::string const& path) :
  MenuNode(parent, label) {
  std::vector<std::string> subs = findSubFolders(path);
  for (std::string &e : subs) {
    std::string buf = extractName(e, gamesavemetaname);
    if (buf.length() == 0) {
      buf = "Corrupted Save";
    }
    addNode(std::make_shared<gameSaveData>(gameSaveData(this, buf, e)));
  }
}

/*******************************************************************************
 *                             gameSaveMenu
 ******************************************************************************/
gameSaveMenu::gameSaveMenu(MenuNode *parent, std::string const& label) :
  MenuNode(parent, label) {
  std::vector<std::string> games = findSubFolders("E:\\UDATA");
  for (std::string &game : games) {
    std::string name = extractName(game, gametitlemetaname);
    if (name.length() == 0) {
      name = "Unknown Game";
    }
    addNode(std::make_shared<gameSave>(gameSave(this, name, game)));
  }
  std::sort(begin(childNodes),
            end(childNodes),
            [](std::shared_ptr<MenuItem> a,
               std::shared_ptr<MenuItem> b) {return a->getLabel() < b->getLabel();});
}
