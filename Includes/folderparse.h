#ifndef FOLDERPARSE_H
#define FOLDERPARSE_H

#ifdef NXDK
#include <xboxkrnl/xboxkrnl.h>
#include <hal/fileio.h>
#endif

#include <stdlib.h>
#include <string.h>

typedef struct _fileData {
  unsigned int f_Attributes;
  unsigned long long int f_CreationTime;
  unsigned long long int f_LastAccessTime;
  unsigned long long int f_LastWriteTime;
  unsigned int f_FileSize;
  char *f_FileName;
} fileData, *pFileData;

HANDLE openFolder(const char *directoryName);

int readFolder(HANDLE handle, pFileData findFileData);

bool closeFolder(HANDLE handle);

#endif
