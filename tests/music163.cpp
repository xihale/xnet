#include "http.hpp"
#include <cstddef>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string_view>
#include "socket.hpp"
#include <xihale/json.hpp>

using namespace xihale::http;
using namespace xihale::socket;
using namespace std;

const umap headers={
    {"Content-Type", "application/x-www-form-urlencoded"},
    {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.142.86 Safari/537.36"},
    {"Referer", "https://music.163.com/"},
    {"Accept", "*/*"},
    {"Accept-Language", "zh-CN,zh;q=0.9"}
};

string getRawUrl(const string &id){
  const string_view url="http://music.163.com/api/song/enhance/player/url";
  auto data=fetch(url,{"POST", 
    "ids=["+id+"]&br=320000",
    headers}
  ).body;
  // auto begin=data.find(R"("url":")");
  // auto end=data.find('"', begin+8);
  clog<<data<<"\n\n\n\n";
  xihale::json::json j(data);
  return j["data"][0]["url"];
  // return data.substr(begin+7, end-begin-7);
}

void fileDownload(const xihale::net::uri::uri &uri, const string &file){

  fstream out(file, ios::binary|ios::out);
  out<<fetch(uri, {
    "GET",
    {},
    headers
  }, 50, 40960).body;
  out.close();

}

int main(){

  // cout<<getRawUrl("33894322")<<'\n';

  // auto url="http://m10.music.126.net/20231111180727/3abc178d0df3a0e87a0ced4d55b87e1c/ymusic/e481/fb0f/fdfb/954e2bd01b5990de4dbe7073a93cd9c5.mp3";
  // auto url="http://127.0.0.1:8080/a.mp4";

  fileDownload(getRawUrl("33894322"), "badapple.mp3");

  return 0;
}
