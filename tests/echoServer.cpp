#include <iostream>
#include "../lib/xsocket.hpp"

using namespace std;

int main()try{

  auto server=xihale::simpleSocket.createServer(6093);

  auto session=server.accept();
  char buf[2049];
  session.read(buf, 2048);
  session.write(buf, 2048);

  return 0;
}catch(const xihale::Exception &e){
  cerr << e.what() << endl;
  return 1;
}
