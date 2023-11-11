#pragma once

// headers start

#include <cstddef>
#include <forward_list>
#include <functional>
#include <string>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/epoll.h>

// headers end

namespace xihale {
namespace socket{

using std::forward_list, std::string;

// types/constants start
typedef int Socket;
typedef sockaddr *LPSockAddr;
typedef sockaddr SockAddr;
static constexpr socklen_t INVALID_SOCKET = -1, SOCKET_ERROR = -1;

enum Defaults { blockSize = 4096 };
// types/constants end

enum modes { tcp, udp };

enum errors {
  Creation,
  Bind,
  Listen,
  Connect,
  Interrupt,
  Accept,
  Domain,
  EpollCreate,
  EpollCtl,
};

// TODO: finish the class
class Exception : public std::exception {
public:
  const errors error;

public:
  Exception(errors e) : error(e) {}
  const char *what() const noexcept {
    switch (error) {
    case Creation:
      return "Create socket error.";
    case Bind:
      return "Bind error.";
    case Listen:
      return "Listen error.";
    case Connect:
      return "Connection error.";
    case Interrupt:
      return "Socket interrupted.";
    case Accept:
      return "Acceptation error.";
    case Domain:
      return "Failed to get ip from domain.";
    case EpollCreate:
      return "Failed to create epoll.";
    case EpollCtl:
      return "Failed to add socket to epoll.";
    }
  }
};

class Utils {
public:
  static forward_list<string> getHostByName(const char *name) {
    struct hostent *host = gethostbyname(name);
    if (!host)
      throw Exception(errors::Domain);
    forward_list<string> hosts;
    for (int i = 0; host->h_addr_list[i]; ++i)
      hosts.push_front(inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));
    return hosts;
  }

  static char *getFirstHostByName(const char *name) {
    struct hostent *host = gethostbyname(name);
    if (!host)
      throw Exception(errors::Domain);
    return inet_ntoa(*(struct in_addr *)host->h_addr_list[0]);
  }

  template <typename T>
  static forward_list<string> getHostByName(const T &name) {
    return getHostByName(name.c_str());
  }
};

class Session {
public:
  Socket session_fd;

public:
  Session(): session_fd(INVALID_SOCKET) {};
  Session(const Socket &socket) : session_fd(socket) {}
  template<typename... Args> Session(Args... args) {connect(args...);}
  ~Session() { close(); }

  size_t read(char *buf, size_t size) noexcept(false) {
    int epoll_fd;
    struct epoll_event event;
    epoll_fd = epoll_create1(0);
    event.events = EPOLLIN;
    event.data.fd = session_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, session_fd, &event);
    epoll_wait(epoll_fd, &event, 1, -1);
    size_t revSize = ::recv(session_fd, buf, size, 0);
    if (revSize == 0)
      throw Exception(errors::Interrupt);
    buf[revSize] = 0x00;
    ::close(epoll_fd);
    return revSize;
  }

  string __read(std::function<const size_t(const size_t &)> maxReadSize,
                size_t block = Defaults::blockSize,
                size_t timeout_ms = 120) noexcept(false) {
    char buf[block + 1];
    size_t revSize;
    string res;
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
      throw Exception(errors::EpollCreate);
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = session_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, session_fd, &event) == -1)
      throw Exception(errors::EpollCtl);

    epoll_event events;
    while (epoll_wait(epoll_fd, &events, 1, timeout_ms) > 0) {
      if ((events.events & EPOLLIN) && events.data.fd == session_fd) {
        revSize = ::recv(session_fd, buf, maxReadSize(res.length()), 0);
        if (revSize == 0)
          throw Exception(errors::Interrupt);
        buf[revSize] = 0x00;
        res += buf;
      }
    }
    ::close(epoll_fd);
    return res;
  }

  string read(size_t size, size_t block = Defaults::blockSize) noexcept(false) {
    return __read([&size](const size_t &len) { return size - len; }, block);
  }

  string readAll(size_t block = Defaults::blockSize) noexcept(false) {
    return __read([&block](const size_t &) { return block; }, block);
  }

  // TODO: Base on Stream
  // TODO: readUntil

  void write(const char *buf, size_t size) noexcept(false) {
    if (::send(session_fd, buf, size, 0) == -1)
      throw Exception(errors::Interrupt);
  }

  template <typename T> void write(const T &buf) noexcept(false) {
    write(buf.c_str(), buf.length());
  }

  void connect(const char *host, size_t port, const modes &mode = tcp) {
    if(session_fd != INVALID_SOCKET) close();
    // judge is host is ip
    auto s_addr = inet_addr(host);
    if (s_addr == INADDR_NONE)
      return connect(Utils::getFirstHostByName(host), port, mode);

    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = s_addr;

    session_fd = ::socket(AF_INET, SOCK_STREAM, mode);
    if (session_fd == INVALID_SOCKET)
      throw Exception(errors::Creation);
    if (::connect(session_fd, (LPSockAddr)&sin, sizeof(sin)) ==
        SOCKET_ERROR)
      throw Exception(errors::Connect);
  }

  void connect(const string &host, size_t port, const modes &mode = tcp) {
    connect(host.c_str(), port, mode);
  }

  void close() { ::close(session_fd); }
};

class SimpleServer {
private:
  Socket server_fd;

public:
  SimpleServer() = default;
  SimpleServer(const int &port, const modes &mode = tcp) { bind(port, mode); }

  void bind(const int &port, const modes &mode = tcp) noexcept(false) {
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    server_fd = ::socket(AF_INET, SOCK_STREAM, mode);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (server_fd == INVALID_SOCKET)
      throw Exception(errors::Creation);
    if (::bind(server_fd, (LPSockAddr)&sin, sizeof(sin)) == SOCKET_ERROR)
      throw Exception(errors::Bind);
    if (::listen(server_fd, 5) == SOCKET_ERROR)
      throw Exception(errors::Listen);
  }

  ~SimpleServer() { close(server_fd); }

  Session accept() noexcept(false) {
    sockaddr_in remoteAddr;
    socklen_t len = sizeof(remoteAddr);
    Socket client = ::accept(server_fd, (LPSockAddr)&remoteAddr, &len);
    if (client == INVALID_SOCKET)
      throw Exception(errors::Accept);
    return client;
  }
};

} // namespace socket
} // namespace xihale