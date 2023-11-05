#include <iostream>
#include "../lib/xsocket.hpp"

using namespace std;

const string msg="Hello Echo!";

int main(){

  auto session = xihale::socket::Session("localhost", 6093);

  char buf[2049];
  session.write(msg);
  session.read(buf, 2048);
  cout<<buf<<'\n';

  return 0;
}
