#include "findXBE.h"
#include <algorithm>
#include <cstdio>

#ifdef NXDK
#include <windows.h>
#include <winnt.h>
#endif

#define XBE_TYPE_MAGIC (0x48454258)
#define XBE_NAME_SIZE 40
#define SECTORSIZE 0x1000

int findXBE(std::string const& path, MenuXbe *list) {
  std::string workPath = path;
  if (workPath.back() != '\\') {
    workPath += '\\';
  }
#ifdef NXDK
  std::string searchmask = workPath + "*";
  std::string tmp;
  char xbeName[XBE_NAME_SIZE + 1];
  char *xbeData = static_cast<char*>(malloc(SECTORSIZE));
  FILE* tmpFILE = nullptr;
  WIN32_FIND_DATAA fData;
  HANDLE fHandle = FindFirstFileA(searchmask.c_str(), &fData);
  if (fHandle == INVALID_HANDLE_VALUE) {
    return -1;
  }

  do {
    if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      tmp = workPath + fData.cFileName + "\\default.xbe";
      tmpFILE = fopen(tmp.c_str(), "rb");
      if (tmpFILE != nullptr) {
        size_t read_bytes = fread(xbeData, 1, SECTORSIZE, tmpFILE);
        PXBE_FILE_HEADER xbe = (PXBE_FILE_HEADER)xbeData;
        if (xbe->SizeOfHeaders > read_bytes) {
          xbeData = static_cast<char*>(realloc(xbeData, xbe->SizeOfHeaders));
          xbe = (PXBE_FILE_HEADER)xbeData;
          read_bytes += fread(&xbeData[read_bytes], 1, xbe->SizeOfHeaders - read_bytes, tmpFILE);
        }
        if(xbe->Magic != XBE_TYPE_MAGIC ||
           xbe->ImageBase != XBE_DEFAULT_BASE ||
           xbe->ImageBase > (uint32_t)xbe->CertificateHeader ||
           (uint32_t)xbe->CertificateHeader + 4 >= (xbe->ImageBase + xbe->SizeOfHeaders) ||
           xbe->SizeOfHeaders > read_bytes) {
          continue;
        }
        PXBE_CERTIFICATE_HEADER xbeCert = (PXBE_CERTIFICATE_HEADER)&xbeData[(uint32_t)xbe->CertificateHeader -
                                                                            xbe->ImageBase];
        memset(xbeName, 0x00, sizeof(xbeName));
        int offset = 0;
        while(offset < XBE_NAME_SIZE) {
          if (xbeCert->TitleName[offset] < 0x0100) {
            sprintf(&xbeName[offset], "%c", (char)xbeCert->TitleName[offset]);
          } else {
            sprintf(&xbeName[offset], "%c", '?');
          }
          if(xbeCert->TitleName[offset] == 0x0000) {
            break;
          }
          ++offset;
        }
        list->addNode(std::make_shared<MenuLaunch>(xbeName, tmp));
        fclose(tmpFILE);
        tmpFILE = nullptr;
      }
    }
  } while (FindNextFile(fHandle, &fData) != 0);
  free(xbeData);
  FindClose(fHandle);
#else
  const char* mask = "*";
  for (int i = 0; i < 7; ++i) {
    list->addNode(std::make_shared<MenuLaunch>(workPath, mask));
  }
#endif
  return 0;
}
