#pragma once

#include "xsocket.hpp"
#include <cstring>
#include <string>
#include <unordered_map>

namespace xihale {
namespace http {

enum errors {
  Https,
};

class Exception : public std::exception {
public:
  errors error;
  Exception(errors e) : error(e) {}

  const char *what() const noexcept {
    switch (error) {
    case Https:
      return "Unsupported https.";
    }
  }
};
static std::string
request(const std::string &method, std::string uri,
        const std::string &data="",
        const std::string &headers="") {
  using std::strncmp, std::string;

  // delete prefix
  if (uri.compare(0, 8, "https://") == 0)
    throw Exception(errors::Https);
  else if (uri.compare(0, 7, "http://") == 0)
    uri.erase(0, 7);

  // divide the hostname and port
  auto mid = uri.find(':');
  auto end = uri.find('/');
  if (end == string::npos)
    end = uri.length();
  if (mid == string::npos)
    mid = end;

  string host = uri.substr(0, mid);
  size_t port =
      end - mid > 1 ? std::stoi(uri.substr(mid + 1, end - mid - 1)) : 80;
  string path = uri.substr(end);
  if(path.empty())
    path = "/";
  if(method=="GET" && !data.empty())
    path += (path.find_first_of('?')==-1?'?':'&')+data;

  // TODO: Trim the uri
  // delete http(s)://
  string raw;
  raw.reserve(1024);
  raw = method + ' ' + path + " HTTP/1.1\r\n" + headers + "Host: "+host+"\r\n";
  if(method=="POST") raw+="Content-Length: "+std::to_string(data.length())+"\r\n";

  raw += "\r\n";

  // data
  if(method=="POST") raw += data;

  auto session=socket::Session(host, port);
  session.write(raw);

  return session.readAll();
}

template <typename ... Args>
static std::string request(const std::string &method, std::string uri, const std::unordered_map<std::string, std::string> &data, Args... args) {
  std::string raw;
  for (auto &it : data)
    raw += it.first + "=" + it.second + "&";
  return request(method, uri, raw, args...);
}

template <typename T>
static std::string request(const std::string &method, std::string uri, const T &data, const std::unordered_map<std::string, std::string> &headers) {
  std::string raw;
  for (auto &it : headers)
    raw += it.first + ": " + it.second + "\r\n";
  return request(method, uri, data, raw);
}
};
}