#include "sntpClient.hpp"
#include <utility>
#include "infoLog.hpp"

// clang-format off
#ifdef NXDK
#include <lwip/netdb.h>

static int sntpConnect(std::string const& sntpHost, uint32_t port, struct sockaddr& saddr);
#endif
// clang-format on

// Delta between 1/1/1970 and 1/1/1900.
static const uint64_t unixToNTPOffsetSeconds = 2208988800ULL;

// Delta between 1/1/1900 and 1/1/1601.
static const uint64_t unixToWindowsOffsetSeconds = 11644473600ULL;

// Conversion from seconds to the 100-nanosecond interval used by the XBOX clock.
static const uint64_t secondsTo100Nanoseconds = 10000000ULL;

sntpClient::sntpClient(std::string host, unsigned int port) :
    host(std::move(host)), port(port) {
}

int sntpClient::threadMain(const sntpClient* client) {
  // TODO: Check periodically (ideally using the polling interval in the first response).
  client->updateTime();
  return 0;
}

void sntpClient::updateTimeAsync() {
  thread = std::thread(threadMain, this);
}

void sntpClient::ntpToNTTime(sntpClient::sntpTimestamp const& ntpTime, uint64_t& ntTime) {
  uint64_t t = ntpTime.seconds;
  t -= unixToNTPOffsetSeconds;
  t += unixToWindowsOffsetSeconds;
  t *= secondsTo100Nanoseconds;

  ntTime = t;
  // TODO: Handle fractional seconds.
}

void sntpClient::updateTime() const {
#ifdef NXDK
  struct sockaddr saddr{};
  int s = sntpConnect(host, port, saddr);

  sntpMessage message{};
  message.version = 3;
  message.mode = SNTP_CLIENT;

  LARGE_INTEGER systemTime;
  // TODO: Set message.transmitTimestamp to the current system time, converted to NTP time.

  socklen_t saddr_len = sizeof(saddr);
  int sent = sendto(s, &message, sizeof(message), 0, (struct sockaddr*)&saddr, saddr_len);
  if (sent != sizeof(message)) {
    InfoLog::outputLine(InfoLog::INFO, "SNTP: Failed to send request message\n");
    close(s);
    return;
  }

  int received = recvfrom(s, &message, sizeof(message), 0, &saddr, &saddr_len);
  close(s);
  if (received != sizeof(message)) {
    InfoLog::outputLine(InfoLog::INFO, "SNTP: Failed to receive response: %d\n", received);
    return;
  }

  message.originTimestamp.seconds = htonl(message.originTimestamp.seconds);
  message.originTimestamp.fractionalSeconds =
      htonl(message.originTimestamp.fractionalSeconds);
  if (message.originTimestamp.seconds || message.originTimestamp.fractionalSeconds) {
    InfoLog::outputLine(InfoLog::INFO, "SNTP: Origin epoch is not 0: %lld\n",
                        message.originTimestamp);
  }

  message.transmitTimestamp.seconds = htonl(message.transmitTimestamp.seconds);

  // TODO: Estimate round trip time using receiveTimestamp and add to transmitTimestamp.

  KeQuerySystemTime(&systemTime);
  uint64_t serverTime64;
  ntpToNTTime(message.transmitTimestamp, serverTime64);
  LARGE_INTEGER serverTime;
  serverTime.QuadPart = static_cast<LONGLONG>(serverTime64);

  LONGLONG delta;
  if (serverTime.QuadPart > systemTime.QuadPart) {
    delta = serverTime.QuadPart - systemTime.QuadPart;
  } else {
    delta = systemTime.QuadPart - serverTime.QuadPart;
  }

  if (delta > allowedDriftSeconds) {
    InfoLog::outputLine(InfoLog::DEBUG,
                        "SNTP: Updating system clock (%llu seconds of drift)\n", delta);
    NTSTATUS status = NtSetSystemTime(&serverTime, nullptr);
    if (!NT_SUCCESS(status)) {
      InfoLog::outputLine(InfoLog::INFO, "SNTP: NtSetSystemTime failed: %X\n", status);
    }
  }

#endif // #ifdef NXDK
}

#ifdef NXDK
static int sntpConnect(std::string const& sntpHost, uint32_t port, struct sockaddr& saddr) {
  if (port > 0xFFFF) {
    InfoLog::outputLine(InfoLog::WARNING, "SNTP: invalid port: %d\n", port);
    return -1;
  }

  struct addrinfo hints{};
  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = IPPROTO_UDP;

  char portString[16] = { 0 };
  snprintf(portString, 15, "%d", port);

  struct addrinfo* ai;
  int status = getaddrinfo(sntpHost.c_str(), portString, &hints, &ai);
  if (status) {
    InfoLog::outputLine(InfoLog::INFO, "SNTP: getaddrinfo failed: %d\n", status);
    return -1;
  }

  int ret;
  ret = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
  if (ret < 0) {
    InfoLog::outputLine(InfoLog::INFO, "SNTP: Failed to create socket: %d\n", errno);
    freeaddrinfo(ai);
    return ret;
  }

  memcpy(&saddr, ai->ai_addr, sizeof(saddr));

  return ret;
}
#endif
