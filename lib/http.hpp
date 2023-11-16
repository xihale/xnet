#pragma once

#include "socket.hpp"
#include "uri.hpp"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

namespace xihale {
namespace http {

typedef std::unordered_map<std::string_view, std::string_view> umap;

enum errors {
  UnsupportedScheme,
};

class Exception : public std::exception {
public:
  errors error;
  Exception(errors e) : error(e) {}

  const char *what() const noexcept override {
    switch (error) {
    case UnsupportedScheme:
      return "Unsupported scheme.";
    }
  }
};

template<bool mode=0>
struct dataVisitor{
  typedef std::string result_type;
  result_type operator()(const result_type &v) { return v; }
  template<class T>
  result_type operator()(const T &v) { 
    std::string raw;
    for (auto &it : v)
      raw += result_type(it.first) + (mode?": ":"=") + result_type(it.second) + (mode?"\r\n":"&");
    return raw;
  }
};

struct options{
  std::string_view method;
  std::variant<std::string, umap> data,  headers;
};

class Response{ // TODO: ignore the case
public:
  std::string headers_raw;
  std::string body;
  umap headers;

private:
  std::string_view trim(std::string_view s){
    s.remove_prefix(std::min(s.find_first_not_of(' '), s.length()));
    return s.substr(0, s.find_last_not_of(' ')+1);
  }
public:

  void parse_headers() {
    size_t pos=0;
    std::string_view h=this->headers_raw;
    const auto &npos=std::string_view::npos;
    // there may be some space in each section
    while((pos=h.find_first_of("\r\n"))!=npos){
      std::string_view line=h.substr(0, pos);
      h.remove_prefix(pos+2);
      pos=line.find_first_of(':');
      if(pos==npos) continue;
      auto key=trim(line.substr(0, pos));
      auto value=trim(line.substr(pos+1));
      this->headers[key]=value;
    }
  }
};

static Response
fetch(const net::uri::uri &uri, const options &opts, const size_t timeout_ms=60, const size_t block=socket::Defaults::blockSize){
  using std::strncmp, std::string, std::string_view, std::visit;
  const string_view &method=opts.method;
  string data=visit(dataVisitor(), opts.data);
  string headers=visit(dataVisitor<1>(), opts.headers);

  if (uri.scheme != "http")
    throw Exception(errors::UnsupportedScheme);

  string raw, host=std::string(uri.host), path=std::string(uri.path);
  raw.reserve(1024);
  raw = method;
  raw += ' ' + path + (method=="GET" && !data.empty()? (uri.path.find_first_of('?')==-1?'?':'&')+data: "") + + " HTTP/1.1\r\n" + headers + "Host: "+host+"\r\n";
  if(method=="POST") raw+="Content-Length: "+std::to_string(data.length())+"\r\n";

  raw += "\r\n";

  // data
  if(method=="POST") raw += data;

  auto session=socket::Session(host.data(), uri.port);
  session.write(raw);

  string headers_raw, res=session.readAll([&headers_raw](string &res, std::optional<size_t> &len, size_t &dataRecv, const size_t &revSize){
    if(len==std::nullopt){
      auto split=res.find("\r\n\r\n");
      headers_raw=res.substr(0, split);
      res.erase(0, split+4);
      std::transform(headers_raw.begin(), headers_raw.end(), headers_raw.begin(), ::tolower);
      if(split!=string::npos){
        auto contentLenPos=headers_raw.find("content-length:");
        if(contentLenPos!=string::npos){
          bool Aspace=(headers_raw[contentLenPos+15]==' ');
          auto contentLen=headers_raw.substr(contentLenPos+15+Aspace, headers_raw.find("\r\n", contentLenPos)-contentLenPos-15-Aspace);
          len=std::stoul(contentLen);
          res.reserve(*len+4);
          dataRecv=revSize-split-4;
        }else len=0;
      }
    }else dataRecv+=revSize;
    if(len!=std::nullopt && dataRecv>=*len) return true;
    return false;
  }, timeout_ms, block);
  return Response{headers_raw, res};
}
};
}