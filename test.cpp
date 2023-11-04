#include <iostream>
#include "./lib/xsocket.hpp"

using namespace std;

const string msg="POST /post HTTP/1.1\r\nHost: httpbin.org\r\nContent-Length: 5\r\n\r\nfoo=1";

int main()try{

  auto session = xihale::simpleSocket.connect("httpbin.org", 80);
  session.write(msg);
  char buf[2049];
  session.read(buf, 2048);
  cout<<buf<<'\n';

  return 0;
}catch(const xihale::Exception &e){
  cerr << e.what() << endl;
  return 1;
}
