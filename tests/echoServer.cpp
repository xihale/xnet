#include <iostream>
#include "socket.hpp"

using namespace std;

int main(){

  auto server=xihale::socket::SimpleServer(6093);

  auto session=server.accept();
  char buf[2049];
  session.read(buf, 2048);
  session.write(buf);

  return 0;
}