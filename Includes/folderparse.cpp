#include "folderparse.h"

#define FIND_DATA_SIZE (sizeof(FILE_DIRECTORY_INFORMATION) + 1024)

HANDLE openFolder(char *directoryName) {
  IO_STATUS_BLOCK IOStatusBlock;
  HANDLE ret = nullptr;
  ANSI_STRING fName;
  OBJECT_ATTRIBUTES Attribs;

  char tmp[200];
  int rc = XConvertDOSFilenameToXBOX(directoryName, tmp);
  if (rc != STATUS_SUCCESS) {
    return nullptr;
  }

  RtlInitAnsiString(&fName, tmp);
  Attribs.RootDirectory = NULL;
  Attribs.ObjectName = &fName;
  Attribs.Attributes = OBJ_CASE_INSENSITIVE;

  NTSTATUS status = NtCreateFile(&ret, FILE_LIST_DIRECTORY, &Attribs, &IOStatusBlock,
                                 nullptr, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ,
                                 FILE_OPEN, FILE_DIRECTORY_FILE);
  if (!NT_SUCCESS(status)) {
    return nullptr;
  }
  return ret;
}

int readFolder(HANDLE handle, pFileData findFileData) {
  IO_STATUS_BLOCK IOStatusBlock;
  BYTE rawFileData[FIND_DATA_SIZE];
  PFILE_DIRECTORY_INFORMATION fileInfo = (PFILE_DIRECTORY_INFORMATION)rawFileData;
  FILE_INFORMATION_CLASS fileInfoClass = FileDirectoryInformation;
  OBJECT_ATTRIBUTES Attribs;

  const char *mask = "*";
  ANSI_STRING fMask;
  
  RtlInitAnsiString(&fMask, mask);

  memset(fileInfo, 0, FIND_DATA_SIZE);
  NTSTATUS status = NtQueryDirectoryFile(
    handle, nullptr, nullptr, nullptr, &IOStatusBlock,
    fileInfo, FIND_DATA_SIZE, fileInfoClass,
    &fMask, false);
  if (!NT_SUCCESS(status)) {
    return ERROR_NO_MORE_FILES;
  }

  findFileData->f_Attributes = fileInfo->FileAttributes;
  findFileData->f_CreationTime = fileInfo->CreationTime.QuadPart;
  findFileData->f_LastAccessTime = fileInfo->LastAccessTime.QuadPart;
  findFileData->f_LastWriteTime = fileInfo->LastWriteTime.QuadPart;
  findFileData->f_FileSize = fileInfo->AllocationSize.QuadPart;
  findFileData->f_FileName = (char*)malloc(fileInfo->FileNameLength + 1);
  memcpy(findFileData->f_FileName, fileInfo->FileName, fileInfo->FileNameLength);
  findFileData->f_FileName[fileInfo->FileNameLength] = '\0';

  return STATUS_SUCCESS;
}

bool closeFolder(HANDLE handle) {
  NTSTATUS status = NtClose(handle);
  return NT_SUCCESS(status);
}

