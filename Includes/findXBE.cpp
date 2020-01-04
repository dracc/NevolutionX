#include "findXBE.h"
#include <algorithm>
#include <cstdio>

#ifdef NXDK
#include <windows.h>
#include "xbe.h"
#endif

#define SECTORSIZE 0x1000

int findXBE(void* list) {
  xbeFinderArg *itm = static_cast<xbeFinderArg*>(list);
  std::vector<xbeMenuItem>* gmi_list = itm->list;
  std::string searchmask = itm->path + "*";
  const char* path = itm->path.c_str();
#ifdef NXDK
  char tmp[64];
  char xbeName[XBENAMESIZE + 1];
  char *xbeData = static_cast<char*>(malloc(SECTORSIZE));
  FILE* tmpFILE = nullptr;
  WIN32_FIND_DATAA fData;
  HANDLE fHandle = FindFirstFileA(searchmask.c_str(), &fData);
  if (fHandle == INVALID_HANDLE_VALUE) {
    return -1;
  }

  do {
    if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      tmp[0] = '\0';
      sprintf(tmp, "%s%s\\default.xbe", path, fData.cFileName);
      tmpFILE = fopen(tmp, "rb");
      if (tmpFILE != nullptr) {
        size_t read_bytes = fread(xbeData, 1, SECTORSIZE, tmpFILE);
        XBE *xbe = (XBE*)xbeData;
        if (xbe->sizeOfHeaders > read_bytes) {
          xbeData = static_cast<char*>(realloc(xbeData, xbe->sizeOfHeaders));
          xbe = (XBE*)xbeData;
          read_bytes += fread(&xbeData[read_bytes], 1, xbe->sizeOfHeaders - read_bytes, tmpFILE);
        }
        if(xbe->type != XBETYPEMAGIC ||
           xbe->baseAddress != BASEADDRESS ||
           xbe->baseAddress > xbe->certAddress ||
           xbe->certAddress + 4 >= (xbe->baseAddress + xbe->sizeOfHeaders) ||
           xbe->sizeOfHeaders > read_bytes) {
          continue;
        }
        XBECert *xbeCert = (XBECert*)&xbeData[xbe->certAddress -
                                              xbe->baseAddress];
        memset(xbeName, 0x00, sizeof(xbeName));
        int offset = 0;
        while(offset < XBENAMESIZE) {
          if (xbeCert->xbeName[offset] < 0x0100) {
            sprintf(&xbeName[offset], "%c", (char)xbeCert->xbeName[offset]);
          } else {
            sprintf(&xbeName[offset], "%c", '?');
          }
          if(xbeCert->xbeName[offset] == 0x0000) {
            break;
          }
          ++offset;
        }
        gmi_list->push_back(xbeMenuItem(xbeName,tmp));
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
    gmi_list->push_back(xbeMenuItem(path, mask));
  }
#endif
  std::sort(gmi_list->begin(), gmi_list->end());
  return 0;
}
