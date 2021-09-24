#include "xbeScanner.h"
#include <windows.h>
#include <winnt.h>
#include "infoLog.h"
#include "timing.h"

#define XBE_TYPE_MAGIC (0x48454258)
#define SECTORSIZE 0x1000

static bool scan(std::string const& path, std::vector<XBEScanner::XBEInfo>& ret);

XBEScanner* XBEScanner::singleton = nullptr;

XBEScanner* XBEScanner::getInstance() {
  if (!XBEScanner::singleton) {
    XBEScanner::singleton = new XBEScanner();
  }
  return XBEScanner::singleton;
}

#if SCANNER_THREADED
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
#endif

void XBEScanner::scanPath(const std::string& path, Callback&& callback) {
#if SCANNER_THREADED
  XBEScanner::getInstance()->addJob(path, callback);
#else
  auto job = QueueItem(path, callback);
  job.scan();
#endif
}

#if SCANNER_THREADED
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
#endif

XBEScanner::QueueItem::QueueItem(std::string p, XBEScanner::Callback c) :
    path(std::move(p)), callback(std::move(c)) {
  xbeData.resize(SECTORSIZE);
}

XBEScanner::QueueItem::~QueueItem() {
  if (dirHandle != INVALID_HANDLE_VALUE) {
    CloseHandle(dirHandle);
  }
}

void XBEScanner::QueueItem::scan() {
  if (dirHandle == INVALID_HANDLE_VALUE) {
    InfoLog::outputLine("Starting scan of %s", path.c_str());
    results.clear();
    scanStart = std::chrono::steady_clock::now();
    if (!openDir()) {
      callback(false, results, millisSince(scanStart));
      return;
    }
  }

  do {
    if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      continue;
    }

    processFile(path + findData.cFileName + "\\default.xbe");
  } while (FindNextFile(dirHandle, &findData));

  CloseHandle(dirHandle);
  dirHandle = INVALID_HANDLE_VALUE;
  callback(true, results, millisSince(scanStart));
}

bool XBEScanner::QueueItem::openDir() {
  std::string searchmask = path + "*";
  dirHandle = FindFirstFile(searchmask.c_str(), &findData);
  return (dirHandle != INVALID_HANDLE_VALUE);
}

void XBEScanner::QueueItem::processFile(const std::string& xbePath) {
  FILE* xbeFile = fopen(xbePath.c_str(), "rb");
  if (!xbeFile) {
    return;
  }

  size_t read_bytes = fread(xbeData.data(), 1, SECTORSIZE, xbeFile);
  auto xbe = (PXBE_FILE_HEADER)xbeData.data();
  if (xbe->SizeOfHeaders > read_bytes) {
    if (xbeData.size() < xbe->SizeOfHeaders) {
      xbeData.resize(xbe->SizeOfHeaders);
    }
    read_bytes += fread(&xbeData[read_bytes], 1, xbe->SizeOfHeaders - read_bytes, xbeFile);
  }
  if (xbe->Magic != XBE_TYPE_MAGIC || xbe->ImageBase != XBE_DEFAULT_BASE
      || xbe->ImageBase > (uint32_t)xbe->CertificateHeader
      || (uint32_t)xbe->CertificateHeader + 4 >= (xbe->ImageBase + xbe->SizeOfHeaders)
      || xbe->SizeOfHeaders > read_bytes) {
    return;
  }
  auto xbeCert =
      (PXBE_CERTIFICATE_HEADER)&xbeData[(uint32_t)xbe->CertificateHeader - xbe->ImageBase];

  for (int offset = 0; offset < XBE_NAME_SIZE; ++offset) {
    if (xbeCert->TitleName[offset] < 0x0100) {
      xbeName[offset] = (char)xbeCert->TitleName[offset];
    } else if (xbeCert->TitleName[offset]) {
      xbeName[offset] = '?';
    } else {
      xbeName[offset] = 0;
      break;
    }
  }

  // Some homebrew content may not have a name in the certification
  // header, so fallback to using the path as the name.
  if (!strlen(xbeName)) {
    strncpy(xbeName, findData.cFileName, sizeof(xbeName) - 1);
  }
  fclose(xbeFile);

  results.emplace_back(xbeName, xbePath);
}