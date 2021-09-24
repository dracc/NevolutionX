#ifndef NEVOLUTIONX_INCLUDES_XBESCANNER_H_
#define NEVOLUTIONX_INCLUDES_XBESCANNER_H_

#include <windows.h>
#include <chrono>
#include <list>
#include <queue>
#include <string>
#include <vector>

// Singleton providing functionality to asynchronously scan a directory for
// direct subdirectories containing XBE files.
class XBEScanner {
public:
  struct XBEInfo {
    XBEInfo(std::string n, std::string p) : name(std::move(n)), path(std::move(p)) {}
    std::string name;
    std::string path;
  };

  // (bool succeeded, std::list<XBEInfo> const& xbes, long long totalScanTime)
  typedef std::function<void(bool, std::list<XBEInfo> const&, long long)> Callback;

  // Enqueues a task to scan `path` for XBE files.
  //
  // On completion of the scan, the given `callback` will be invoked with a
  // boolean indicating whether the scan succeeded or not and a vector of
  // XBEInfo instances for any XBEs that were discovered.
  static void scanPath(std::string const& path, Callback&& callback);

  // Starts or continues a scan, using `timeoutMilliseconds` as a soft upper limit on
  // processing time.
  static void tick(int timeoutMilliseconds) { getInstance()->process(timeoutMilliseconds); }

private:
  class QueueItem {
  public:
    QueueItem(std::string p, Callback c);
    ~QueueItem();

    // Begins or continues a scan. Returns "true" if the scan is completed and the callback
    // has been invoked, false if the scan timed out, indicating that `scan` should be
    // called again.
    bool scan(int timeoutMillis);

    std::chrono::steady_clock::time_point scanStart;
    long long scanDuration{ 0 };

    const static int XBE_NAME_SIZE = 40;

    std::string path;
    Callback callback;
    std::list<XBEInfo> results;

    bool openDir();
    void processFile(const std::string& xbePath);

    HANDLE dirHandle{ INVALID_HANDLE_VALUE };
    WIN32_FIND_DATAA findData{};

    char xbeName[XBE_NAME_SIZE + 1]{ 0 };
    std::vector<char> xbeData;
  };

  static XBEScanner* getInstance();

  void addJob(std::string const& path, const Callback& callback);
  void process(int timeoutMilliseconds);

  static XBEScanner* singleton;

  std::queue<QueueItem> queue;
};


#endif // NEVOLUTIONX_INCLUDES_XBESCANNER_H_
