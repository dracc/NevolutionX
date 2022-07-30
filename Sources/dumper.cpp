#include "dumper.hpp"
#ifdef NXDK
#include <hal/debug.h>
#include <winapi/fileapi.h>
#include <xboxkrnl/xboxkrnl.h>
#include <fstream>
#endif
#include "infoLog.hpp"
#include "menu.hpp"

#define BIOSAREA 0xFFF00000
#define BIOSSIZE (256 * 1024)

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
    /* Dump BIOS */
    debugPrint("Creating file bios.bin\n");
    std::ofstream BiosDump("C:\\Backup\\bios.bin",
                           std::ofstream::trunc | std::ofstream::binary);
    char* biosArea = static_cast<char*>(MmMapIoSpace(BIOSAREA, BIOSSIZE, PAGE_READONLY));
    debugPrint("Writing to file\n");
    BiosDump.write(biosArea, BIOSSIZE);
    MmUnmapIoSpace(biosArea, BIOSSIZE);
    InfoLog::outputLine(InfoLog::INFO, "Dumped BIOS to C:\\Backup\\bios.bin");

    /* Dump EEPROM */
    char buffer[0x100];
    uint16_t* it = (uint16_t*)buffer;
    for (size_t i = 0; i < sizeof(buffer); i += 2) {
      ULONG value;
      HalReadSMBusValue(0xA9, i, true, &value);
      *it++ = (uint16_t)value;
    }
    std::ofstream eepromDump("C:\\Backup\\eeprom.bin",
                             std::ofstream::trunc | std::ofstream::binary);
    eepromDump.write(buffer, 0x100);
    InfoLog::outputLine(InfoLog::INFO, "Dumped EEPROM to C:\\Backup\\eeprom.bin");
  }
  FindClose(backupFolder);
#endif
}
