#include "dumper.hpp"
#ifdef NXDK
#include <winapi/fileapi.h>
#include <fstream>
#endif
#include "infoLog.hpp"
#include "menu.hpp"

#define BIOSAREA ((void*)0xFFF00000)

Dumper::Dumper(std::string const& label) : MenuItem(label) {
}

void Dumper::execute(Menu*) {
#ifdef NXDK
  const std::string backupPath = "C:\\Backup";
  HANDLE backupFolder = CreateFileA(
      backupPath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);
  if (backupFolder == INVALID_HANDLE_VALUE) {
    CreateDirectory(backupPath.c_str(), nullptr);
    backupFolder = CreateFileA(backupPath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);
  }
  if (backupFolder == INVALID_HANDLE_VALUE) {
    InfoLog::outputLine(InfoLog::WARNING, "C:\\Backup folder not found nor created!");
  } else {
    std::ofstream BiosDump("C:\\Backup\\bios.bin",
                           std::ofstream::binary | std::ofstream::trunc);
    BiosDump.write(static_cast<char*>(BIOSAREA), 256 * 1024);
    InfoLog::outputLine(InfoLog::INFO, "Dumped BIOS to C:\\Backup\\bios.bin");
  }
  FindClose(backupFolder);
#endif
}
