#include "dumper.hpp"
#include <winapi/fileapi.h>
#include <fstream>
#include "infoLog.hpp"
#include "menu.hpp"

#define BIOSAREA ((void*)0xFFFC0000)

Dumper::Dumper(std::string const& label) : MenuItem(label) {
}

void Dumper::execute(Menu*) {
  WIN32_FIND_DATAA fData;
  HANDLE fHandle = FindFirstFileA("C:\\Backup", &fData);
  if (fHandle == INVALID_HANDLE_VALUE) {
    CreateDirectory("C:\\Backup", nullptr);
    fHandle = FindFirstFileA("C:\\Backup", &fData);
  }
  if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
    std::ofstream BiosDump("C:\\Backup\\bios.bin", std::ofstream::binary);
    BiosDump.write(static_cast<char*>(BIOSAREA), 256 * 1024);
    BiosDump.flush();
    BiosDump.close();
    InfoLog::outputLine(InfoLog::INFO, "Dumped BIOS to C:\\Backup\\bios.bin");
  } else {
    InfoLog::outputLine(InfoLog::WARNING, "C:\\Backup folder not found nor created!");
  }
  FindClose(fHandle);
}
