#include <cstdio>
#include <iostream>
#include <unistd.h>
#include "socket.hpp"

using namespace std;

const string msg="Hello Echo!";

int main(){

  xihale::socket::Session session ;

  while(true){
    try{
      session.connect("localhost", 6093);
    }catch(...){
      continue;
    }
    break;
  }

  char buf[2049];
  session.write(msg);
  auto revSize=session.read(buf, 2048);
  // write(STDOUT_FILENO, buf, revSize);
  cout.write(buf, revSize);
  cout.put('\n');

  return 0;
}
