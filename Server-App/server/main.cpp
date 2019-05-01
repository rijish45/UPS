#include "socket.hpp"
#include "worldMsg.hpp"
#include "thread.hpp"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <thread> 

using namespace std;

int main() {
  setUpEnvironment(); 
  thread th1(worldSide); 
  thread th2(amazonSide);
  thread th3(updateAddress);
  thread th4(recvAdreUpdatedACK);
  th1.join();
  th2.join();
  th3.join();
  th4.join();
  cout << "Hello" << endl;
  while (true) {
  }
}
