#include "findXBE.h"

int findXBE(void* list) {
  xbeFinderArg *itm = static_cast<xbeFinderArg*>(list);
  vector<xbeMenuItem>* gmi_list = itm->list;
  char* path = const_cast<char*>(itm->path);//("C:\\");
  char* mask = const_cast<char*>("*");
#ifdef NXDK
  int ret;
  char tmp[64];
  FILE* tmpFILE = nullptr;
  fileData fData;
  HANDLE fHandle = openFolder(path);
  if (fHandle == nullptr) {
    return -1;
  }

  while (readFolder(fHandle, &fData) == STATUS_SUCCESS) {
    if (fData.f_Attributes & FILE_ATTRIBUTE_DIRECTORY) {
      tmp[0] = '\0';
      sprintf(tmp, "%s%s\\default.xbe", path, fData.f_FileName);
      tmpFILE = fopen(tmp, "rb");
      if (tmpFILE != nullptr) {
        xbeMenuItem* itm = new xbeMenuItem(fData.f_FileName, tmp);
        gmi_list->push_back(*itm);
        fclose(tmpFILE);
        tmpFILE = nullptr;
      }
    }
  }
  closeFolder(fHandle);
#else
  for (int i = 0; i < 7; ++i) {
    gmi_list->push_back(xbeMenuItem(path, mask));
  }
#endif
  return 0;
}
