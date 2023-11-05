#include <iostream>
#include "../lib/xhttp.hpp"

using namespace std;

int main(){

  cout<<xihale::http::request("GET", "httpbin.org/get?bar=2");
  cout<<xihale::http::request("GET", "httpbin.org/get?bar=2", {
    {"foo", "1"},
  });
  cout<<xihale::http::request("POST", "httpbin.org/post", "foo=1&bar=2", {
    {"User-Agent", "xhttp"},
  });

  cout<<xihale::http::request("POST", "http://music.163.com/api/song/enhance/player/url", "ids=[33894322]&br=320000", {
    {"Content-Type", "application/x-www-form-urlencoded"},
    {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.142.86 Safari/537.36"},
    {"Referer", "https://music.163.com/"},
    {"Accept", "*/*"},
    {"Accept-Language", "zh-CN,zh;q=0.9"}
  });

  return 0;
}
