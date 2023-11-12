#include "../lib/xhttp.hpp"
#include <iostream>
#include <string_view>
#include <unordered_map>

using namespace std;

int main() {

  using xihale::http::json_t;

  // cout << xihale::http::fetch("httpbin.org/get?bar=2", {"GET", {}, {}});
  // cout << xihale::http::fetch("httpbin.org/get?bar=2",
  //                             {"GET", json_t{{"foo", "1"}}, {}});
  // cout<<xihale::http::fetch("httpbin.org/post", {"POST",
  //  "foo=1&bar=2", 
  //  json_t{
  //   {"User-Agent", "xhttp"},
  // }});

  // cout<<xihale::http::fetch("http://music.163.com/api/song/enhance/player/url",
  // {"POST", "ids=[33894322]&br=320000", json_t{
  //   {"Content-Type", "application/x-www-form-urlencoded"},
  //   {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.142.86 Safari/537.36"},
  //   {"Referer", "https://music.163.com/"},
  //   {"Accept", "*/*"},
  //   {"Accept-Language", "zh-CN,zh;q=0.9"}
  // }});

  return 0;
}
