#ifndef NEVOLUTIONX_INCLUDES_SNTPCLIENT_H_
#define NEVOLUTIONX_INCLUDES_SNTPCLIENT_H_

#include <cstdint>
#include <string>
#include <thread>

class sntpClient {
public:
  sntpClient(std::string sntpHost, uint32_t port);

  void updateTimeAsync();
  void updateTime() const;

private:
  enum sntpMode
  {
    SNTP_SYMMETRIC_ACTIVE = 1,
    SNTP_SYMMETRIC_PASSIVE,
    SNTP_CLIENT,
    SNTP_SERVER,
    SNTP_BROADCAST
  };

  struct sntpTimestamp {
    uint32_t seconds;
    uint32_t fractionalSeconds;
  };

  struct sntpMessage {
    uint8_t mode : 3, version : 3, leapIndicator : 2;
    uint8_t stratum;
    int8_t poll;
    uint8_t precision;
    int32_t rootDelay;
    int32_t rootDispersion;
    int32_t referenceIdentifier;
    sntpTimestamp referenceTimestamp;
    sntpTimestamp originTimestamp;
    sntpTimestamp receiveTimestamp;
    sntpTimestamp transmitTimestamp;
  };

  static void ntpToNTTime(sntpTimestamp const& ntpTime, uint64_t& ntTime);
  static int threadMain(sntpClient const* client);

  std::thread thread;

  std::string host;
  uint32_t port;

  // Maximum number of seconds allowed between NTP and local time before correction is
  // applied.
  uint32_t allowedDriftSeconds{ 5 };
};

#endif // NEVOLUTIONX_INCLUDES_SNTPCLIENT_H_
