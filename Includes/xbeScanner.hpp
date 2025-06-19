#ifndef NEVOLUTIONX_INCLUDES_XBESCANNER_H_
#define NEVOLUTIONX_INCLUDES_XBESCANNER_H_

#include <atomic>
#include <functional>
#include <list>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#ifdef NXDK
#include <windows.h>
#endif

// TODO(#110): Reenable threading once hardware accelerated rendering is in place.
// The current software-backed SDL approach causes the scanner thread to be starved, leading
// to extremely long load times.

// #define SCANNER_THREADED

// Singleton providing functionality to asynchronously scan a directory for
// direct subdirectories containing XBE files.
class XBEScanner {
public:
  struct XBEInfo {
    XBEInfo(std::string n, std::string p) : name(std::move(n)), path(std::move(p)) {}
    std::string name;
    std::string path;
  };

  // (bool succeeded, std::list<XBEInfo> const& xbes, long long scanDuration)
  typedef std::function<void(bool, std::list<XBEInfo> const&, long long)> Callback;

  // Enqueues a task to scan `path` for XBE files.
  //
  // On completion of the scan, the given `callback` will be invoked with a
  // boolean indicating whether the scan succeeded or not and a vector of
  // XBEInfo instances for any XBEs that were discovered.
  static void scanPath(std::string const& path, Callback&& callback);

private:
  class QueueItem {
  public:
    QueueItem(std::string p, Callback c);
    ~QueueItem();

    void scan();

    std::chrono::steady_clock::time_point scanStart;
    long long scanDuration{ 0 };

    const static int XBE_NAME_SIZE = 40;

    std::string path;
    Callback callback;
    std::list<XBEInfo> results;

    bool openDir();
    void processFile(const std::string& xbePath);

#ifdef NXDK
    HANDLE dirHandle{ INVALID_HANDLE_VALUE };
    WIN32_FIND_DATAA findData{};
#endif

    char xbeName[XBE_NAME_SIZE + 1]{ 0 };
    std::vector<char> xbeData;
  };

#ifdef SCANNER_THREADED
  XBEScanner();
  ~XBEScanner();

  static void threadMain(XBEScanner* scanner);
#endif

  static XBEScanner* getInstance();

#if SCANNER_THREADED
  void addJob(std::string const& path, const Callback& callback);
#endif

  static XBEScanner* singleton;

#ifdef SCANNER_THREADED
  std::atomic<bool> running;
  std::thread scannerThread;
  std::condition_variable jobPending;
  std::mutex queueMutex;
  std::queue<QueueItem> queue;
#endif
};


#endif // NEVOLUTIONX_INCLUDES_XBESCANNER_H_
