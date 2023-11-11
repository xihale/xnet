#include "../lib/uri.hpp"
#include <stdexcept>
#include <sstream>
#include <string_view>

using namespace xihale::net;

template <typename T>
void assert_equal(const T &a, const T &b, const std::string_view &msg=""){
  if(a!=b){
    std::stringstream err;
    err<<'`'<<msg<<"` : "<<' '<<a<<' '<<b;
    throw std::runtime_error(err.str());
  }
}

void judge(const std::string_view &msg, const uri::uri &a, const uri::uri &b){
  assert_equal(a.scheme, b.scheme, msg);
  assert_equal(a.host, b.host, msg);
  assert_equal(a.port, b.port, msg);
  assert_equal(a.path, b.path, msg);
}

int main(){

  // complete

  judge("complete", uri::parse("https://example.com:443/path"), {
      .scheme="https",
      .host="example.com",
      .port=443,
      .path="/path"
  });

  // omitted

  // scheme and port
  judge("scheme and port", uri::parse("example.com/path"), {
      .scheme="http",
      .host="example.com",
      .port=80,
      .path="/path"
  });

  // port
  judge("port", uri::parse("https://example.com/path"), {
      .scheme="https",
      .host="example.com",
      .port=443,
      .path="/path"
  });

  // path
  judge("path", uri::parse("https://example.com:443"), {
      .scheme="https",
      .host="example.com",
      .port=443,
      .path="/"
  });

  // scheme, port and path
  judge("scheme, port and path", uri::parse("example.com"), {
      .scheme="http",
      .host="example.com",
      .port=80,
      .path="/"
  });

  return 0;
}