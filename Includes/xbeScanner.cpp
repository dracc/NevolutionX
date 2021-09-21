#include "xbeScanner.h"

#ifdef NXDK
#include <windows.h>
#include <winnt.h>
#endif

#define XBE_TYPE_MAGIC (0x48454258)
#define XBE_NAME_SIZE 40
#define SECTORSIZE 0x1000

static bool scan(std::string const& path, std::vector<XBEScanner::XBEInfo>& ret);

XBEScanner* XBEScanner::singleton = nullptr;

XBEScanner* XBEScanner::getInstance() {
  if (!XBEScanner::singleton) {
    XBEScanner::singleton = new XBEScanner();
  }
  return XBEScanner::singleton;
}

XBEScanner::XBEScanner() : running(true) {
  scannerThread = std::thread(threadMain, this);
}

XBEScanner::~XBEScanner() {
  running = false;
  queue.emplace("", nullptr);
  if (scannerThread.joinable()) {
    scannerThread.join();
  }
}

void XBEScanner::scanPath(const std::string& path, Callback&& callback) {
  XBEScanner::getInstance()->addJob(path, callback);
}

void XBEScanner::addJob(std::string const& path, const Callback& callback) {
  std::lock_guard<std::mutex> lock(queueMutex);
  queue.emplace(path, callback);
  jobPending.notify_one();
}

void XBEScanner::threadMain(XBEScanner* scanner) {
  while (scanner->running) {
    QueueItem task;
    {
      std::unique_lock<std::mutex> lock(scanner->queueMutex);
      scanner->jobPending.wait(lock, [=] { return !scanner->queue.empty(); });
      if (!scanner->running) {
        break;
      }
      task = scanner->queue.front();
      scanner->queue.pop();
    }

    std::string const& path = task.first;
    std::vector<XBEInfo> xbes;
    bool succeeded = scan(path, xbes);

    task.second(succeeded, xbes);
  }
}

static bool scan(std::string const& path, std::vector<XBEScanner::XBEInfo>& ret) {
#ifndef NXDK
  return FALSE;
#else
  std::string workPath = path;
  if (workPath.back() != '\\') {
    workPath += '\\';
  }

  std::string searchmask = workPath + "*";
  std::string xbePath;
  char xbeName[XBE_NAME_SIZE + 1];
  char* xbeData = static_cast<char*>(malloc(SECTORSIZE));
  FILE* tmpFILE = nullptr;
  WIN32_FIND_DATAA fData;
  HANDLE fHandle = FindFirstFileA(searchmask.c_str(), &fData);
  if (fHandle == INVALID_HANDLE_VALUE) {
    return FALSE;
  }

  do {
    if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      xbePath = workPath + fData.cFileName + "\\default.xbe";
      tmpFILE = fopen(xbePath.c_str(), "rb");
      if (tmpFILE != nullptr) {
        size_t read_bytes = fread(xbeData, 1, SECTORSIZE, tmpFILE);
        PXBE_FILE_HEADER xbe = (PXBE_FILE_HEADER)xbeData;
        if (xbe->SizeOfHeaders > read_bytes) {
          xbeData = static_cast<char*>(realloc(xbeData, xbe->SizeOfHeaders));
          xbe = (PXBE_FILE_HEADER)xbeData;
          read_bytes += fread(&xbeData[read_bytes], 1, xbe->SizeOfHeaders - read_bytes,
                              tmpFILE);
        }
        if (xbe->Magic != XBE_TYPE_MAGIC || xbe->ImageBase != XBE_DEFAULT_BASE
            || xbe->ImageBase > (uint32_t)xbe->CertificateHeader
            || (uint32_t)xbe->CertificateHeader + 4 >= (xbe->ImageBase + xbe->SizeOfHeaders)
            || xbe->SizeOfHeaders > read_bytes) {
          continue;
        }
        PXBE_CERTIFICATE_HEADER xbeCert =
            (PXBE_CERTIFICATE_HEADER)&xbeData[(uint32_t)xbe->CertificateHeader
                                              - xbe->ImageBase];
        memset(xbeName, 0x00, sizeof(xbeName));
        int offset = 0;
        while (offset < XBE_NAME_SIZE) {
          if (xbeCert->TitleName[offset] < 0x0100) {
            sprintf(&xbeName[offset], "%c", (char)xbeCert->TitleName[offset]);
          } else {
            sprintf(&xbeName[offset], "%c", '?');
          }
          if (xbeCert->TitleName[offset] == 0x0000) {
            break;
          }
          ++offset;
        }

        // Some homebrew content may not have a name in the certification
        // header, so fallback to using the path as the name.
        if (!strlen(xbeName)) {
          strncpy(xbeName, fData.cFileName, sizeof(xbeName) - 1);
        }
        fclose(tmpFILE);
        tmpFILE = nullptr;

        ret.emplace_back(xbeName, xbePath);
      }
    }
  } while (FindNextFile(fHandle, &fData) != 0);
  free(xbeData);
  FindClose(fHandle);
#endif // #ifndef NXDK

  return TRUE;
}
