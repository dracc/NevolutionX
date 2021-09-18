#ifndef NEVOLUTIONX_INCLUDES_XBESCANNER_H_
#define NEVOLUTIONX_INCLUDES_XBESCANNER_H_

#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
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

  // (bool succeeded, std::vector<XBEInfo> const& xbes)
  typedef std::function<void(bool, std::vector<XBEInfo> const&)> Callback;

  // Enqueues a task to scan `path` for XBE files.
  //
  // On completion of the scan, the given `callback` will be invoked with a
  // boolean indicating whether the scan succeeded or not and a vector of
  // XBEInfo instances for any XBEs that were discovered.
  static void scanPath(std::string const& path, Callback&& callback);

private:
  typedef std::pair<std::string, Callback> QueueItem;

  XBEScanner();
  ~XBEScanner();

  static void threadMain(XBEScanner *scanner);
  static XBEScanner *getInstance();

  void addJob(std::string const& path, const Callback& callback);

  static XBEScanner *singleton;

  std::atomic<bool> running;
  std::thread scannerThread;
  std::condition_variable jobPending;
  std::mutex queueMutex;
  std::queue<QueueItem> queue;
};


#endif //NEVOLUTIONX_INCLUDES_XBESCANNER_H_
