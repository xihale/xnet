#pragma once

// headers start

#include <bits/types/struct_timeval.h>
#include <cstddef>
#include <forward_list>
#include <functional>
#include <string>
#include <sys/select.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

// headers end

namespace xihale {

using std::forward_list, std::string;

// types/constants start
#ifdef WIN32
typedef int socklen_t;
typedef SOCKET Socket;
typedef LPSOCKADDR LPSockAddr;
typedef SOCKADDR SockAddr;
#else
typedef int Socket;
typedef sockaddr *LPSockAddr;
typedef sockaddr SockAddr;
static constexpr socklen_t INVALID_SOCKET = -1, SOCKET_ERROR = -1;
#endif

enum Defaults{
	blockSize=4096
};
// types/constants end

enum modes { XSOCKET_TCP, XSOCKET_UDP };

enum errors {
  Creation,
  Bind,
  Listen,
  Connection,
  Interruption,
  Acceptation,
  Domain,
};

// TODO: finish the class
class Exception : public std::exception {
public:
  const errors error;

public:
  Exception(errors e) : error(e) {}
  const char *what() const noexcept {
    switch (error) {
    case Creation: return "Create socket error.";
		case Bind: return "Bind error.";
		case Listen: return "Listen error.";
		case Connection: return "Connection error.";
		case Interruption: return "Socket interrupted.";
		case Acceptation: return "Acceptation error.";
		case Domain: return "Failed to get ip from domain.";
    }
  }
};

class Session {
public:
  Socket socket;

public:
	Session() = default;
  Session(const Socket &socket) : socket(socket) {}
  ~Session() {
#ifdef WIN32
    closesocket(socket);
#else
    close(socket);
#endif
  }

  int read(char *buf, size_t size) noexcept(false) {
		fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(socket, &readfds);
		// wait for data available
		select(socket+1, &readfds, nullptr, nullptr, nullptr);
    int revSize = ::recv(socket, buf, size, 0);
    // if (!((revSize > 0) || ((revSize == -1) && (socket == EWOULDBLOCK))))
		if (revSize == 0)
      throw Exception(errors::Interruption);
    buf[revSize] = 0x00;
    return revSize;
  }

	string __read(std::function<const size_t(const size_t&)>maxReadSize, size_t block = Defaults::blockSize, timeval timeout={3,0}) noexcept(false) {
		char buf[block+1];
    size_t revSize;
    string str;
    fd_set fds;
		FD_ZERO(&fds);
		FD_SET(socket, &fds);
		while(select(socket+1, &fds, nullptr, nullptr, &timeout) > 0){
			int revSize = ::recv(socket, buf, maxReadSize(str.length()), 0);
			if (revSize == 0)
				throw Exception(errors::Interruption);
			buf[revSize] = 0x00;
			str += buf;
		}
		return str;
	}

  string read(size_t size, size_t block = Defaults::blockSize, timeval timeout={3,0}) noexcept(false) {
    return __read([&size](const size_t &len){return size-len;}, block, timeout);
  }

	string readAll(size_t block = Defaults::blockSize, timeval timeout={3,0}) noexcept(false) {
		return __read([&block](const size_t&){return block;}, block, timeout);
	}

	// TODO: Base on Stream
	// TODO: readUntil

	void write(const char *buf, size_t size) noexcept(false) {
		if (::send(socket, buf, size, 0) == -1)
			throw Exception(errors::Interruption);
	}

	template<typename T> void write(const T &buf) noexcept(false) {
		write(buf.c_str(), buf.length());
	}

};

class SimpleServer {
private:
  Socket socket;

public:
  SimpleServer(const int &port, const modes &mode = XSOCKET_TCP) noexcept(false) {
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
#ifdef WIN32
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    sin.sin_addr.s_addr = INADDR_ANY;
#endif
    socket = ::socket(AF_INET, SOCK_STREAM, mode);
    if (socket == INVALID_SOCKET)
      throw Exception(errors::Creation);
    if (bind(socket, (LPSockAddr)&sin, sizeof(sin)) == SOCKET_ERROR)
      throw Exception(errors::Bind);
    if (::listen(socket, 5) == SOCKET_ERROR)
      throw Exception(errors::Listen);
  }

  ~SimpleServer() {
#ifdef WIN32
    closesocket(socket);
#else
    close(socket);
#endif
  }

  Session accept() noexcept(false) {
    sockaddr_in remoteAddr;
    socklen_t len = sizeof(remoteAddr);
    Socket client = ::accept(socket, (LPSockAddr)&remoteAddr, &len);
    if (client == INVALID_SOCKET)
      throw Exception(errors::Acceptation);
    return client;
  }
};

class SimpleSocket {
public:
  SimpleSocket() {
#ifdef WIN32
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
      throw "Cannot initialize winsock(WSAStartup)";
#endif
  }

  ~SimpleSocket() {
#ifdef WIN32
    WSACleanup();
#endif
  }

  forward_list<string> getHostByName(const char *name) {
    struct hostent *host = gethostbyname(name);
    if (!host)
      throw Exception(errors::Domain);
    forward_list<string> hosts;
    for (int i = 0; host->h_addr_list[i]; ++i)
      hosts.push_front(inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));
    return hosts;
  }

  char *getFirstHostByName(const char *name) {
    struct hostent *host = gethostbyname(name);
    if (!host)
      throw Exception(errors::Domain);
    return inet_ntoa(*(struct in_addr *)host->h_addr_list[0]);
  }

  template <typename T> forward_list<string> getHostByName(const T &name) {
    return getHostByName(name.c_str());
  }

  SimpleServer createServer(int port) { return SimpleServer(port); }

  Session connect(const char *host, int port, const modes &mode = XSOCKET_TCP) {
    // judge is host is ip
    auto s_addr = inet_addr(host);
    if (s_addr == INADDR_NONE)
      return connect(getFirstHostByName(host), port, mode);

    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

#ifdef WIN32
    sin.sin_addr.S_un.S_addr = s_addr;
#else
    sin.sin_addr.s_addr = s_addr;
#endif

    Session session = ::socket(AF_INET, SOCK_STREAM, mode);
    if (session.socket == INVALID_SOCKET)
      throw Exception(errors::Creation);
    if (::connect(session.socket, (LPSockAddr)&sin, sizeof(sin)) ==
        SOCKET_ERROR)
      throw Exception(errors::Connection);
    return session;
  }
};
static SimpleSocket simpleSocket;
} // namespace xihale