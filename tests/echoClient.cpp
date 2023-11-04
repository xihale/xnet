#include <iostream>
#include "../lib/xsocket.hpp"

using namespace std;

const string msg="Hello Echo!";

int main()try{

  auto session = xihale::simpleSocket.connect("localhost", 6093);

  char buf[2049];
  session.write(msg);
  session.read(buf, 2048);
  cout<<buf<<'\n';

  return 0;
}catch(const xihale::Exception &e){
  cerr << e.what() << endl;
  return 1;
}
