#include "uri.hpp"
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
      "https",
      "example.com",
      443,
      "/path"
  });

  // trim spaces
  judge("trim spaces", uri::parse("    https://example.com:443/path    "), {
      "https",
      "example.com",
      443,
      "/path"
  });

  // omitted

  // scheme and port
  judge("scheme and port", uri::parse("example.com/path"), {
      "http",
      "example.com",
      80,
      "/path"
  });

  // port
  judge("port", uri::parse("https://example.com/path"), {
      "https",
      "example.com",
      443,
      "/path"
  });

  // path
  judge("path", uri::parse("https://example.com:443"), {
      "https",
      "example.com",
      443,
      "/"
  });

  // scheme, port and path
  judge("scheme, port and path", uri::parse("example.com"), {
      "http",
      "example.com",
      80,
      "/"
  });

  return 0;
}