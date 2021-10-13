#include "xbeScanner.h"
#include <chrono>
#ifdef NXDK
#include <windows.h>
#include <winnt.h>
#endif

#include "infoLog.h"
#include "timing.h"

#define XBE_TYPE_MAGIC (0x48454258)
#define SECTORSIZE 0x1000

#ifdef NXDK
static std::pair<DWORD, DWORD> getSaveImageFileOffset(FILE* file,
                                                      DWORD imageBase,
                                                      PXBE_SECTION_HEADER firstSectionHeader,
                                                      DWORD numberOfSections);
#endif

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
    auto scanStart = std::chrono::steady_clock::now();
    bool succeeded = scan(path, xbes);
    auto scanDuration = millisSince(scanStart);
    InfoLog::outputLine("Scanning %s %s in %lld ms (%d found)\n", path.c_str(),
                        succeeded ? "succeeded" : "failed", scanDuration, xbes.size());

    task.second(succeeded, xbes);
  }
}
#endif

XBEScanner::QueueItem::QueueItem(std::string p, XBEScanner::Callback c) :
    path(std::move(p)), callback(std::move(c)) {
  xbeData.resize(SECTORSIZE);
}

XBEScanner::QueueItem::~QueueItem() {
#ifdef NXDK
  if (dirHandle != INVALID_HANDLE_VALUE) {
    CloseHandle(dirHandle);
  }
#endif
}

void XBEScanner::QueueItem::scan() {
#ifdef NXDK
  if (dirHandle == INVALID_HANDLE_VALUE) {
    InfoLog::outputLine(InfoLog::INFO, "Starting scan of %s", path.c_str());
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
#endif
}

bool XBEScanner::QueueItem::openDir() {
#ifdef NXDK
  std::string searchmask = path + "*";
  dirHandle = FindFirstFile(searchmask.c_str(), &findData);
  return (dirHandle != INVALID_HANDLE_VALUE);
#else
  return false;
#endif
}

void XBEScanner::QueueItem::processFile(const std::string& xbePath) {
#ifdef NXDK
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

  auto firstSectionHeader = reinterpret_cast<PXBE_SECTION_HEADER>(
      xbeData.data() + (DWORD)xbe->PointerToSectionTable - xbe->ImageBase);
  std::pair<int, int> saveImageInfo = getSaveImageFileOffset(
      xbeFile, xbe->ImageBase, firstSectionHeader, xbe->NumberOfSections);

  fclose(xbeFile);

  results.emplace_back(xbeName, xbePath, saveImageInfo.first, saveImageInfo.second);
#endif // #ifdef NXDK
}

#ifdef NXDK
// Retrieves the FileAddress and FileSize members of the "$$XTIMAGE" section, which points
// to an XPR0 compressed icon for save games.
//
// NOTE: This will seek within the given file, if it is important to maintain the current
//       read position it should be saved before calling this function.
static std::pair<DWORD, DWORD> getSaveImageFileOffset(FILE* file,
                                                      DWORD imageBase,
                                                      PXBE_SECTION_HEADER firstSectionHeader,
                                                      DWORD numberOfSections) {
  static const char SAVE_IMAGE_SECTION_NAME[] = "$$XTIMAGE";
  static const int SECTION_NAME_SIZE = sizeof(SAVE_IMAGE_SECTION_NAME);

  char nameBuffer[SECTION_NAME_SIZE] = { 0 };
  for (DWORD i = 0; i < numberOfSections; ++i) {
    PXBE_SECTION_HEADER header = firstSectionHeader + i;
    long nameOffset = reinterpret_cast<long>(header->SectionName) - imageBase;
    fseek(file, nameOffset, SEEK_SET);
    size_t read_bytes = fread(nameBuffer, 1, SECTION_NAME_SIZE, file);
    if (read_bytes != SECTION_NAME_SIZE) {
      return std::make_pair(-1, -1);
    }

    if (nameBuffer[SECTION_NAME_SIZE - 1] == 0
        && !strcmp(nameBuffer, SAVE_IMAGE_SECTION_NAME)) {
      return std::make_pair(header->FileAddress, header->FileSize);
    }
  }

  return std::make_pair(-1, -1);
}
#endif // #ifdef NXDK
