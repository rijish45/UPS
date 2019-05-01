#ifndef __SOCKET__
#define __SOCKET__

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <netdb.h>
#include <utility>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include "world_ups.pb.h"
#include "UA.pb.h"

#define BUF_SIZE 812 
#define CH_SIZE 1

using namespace std;

class Socket {
protected:
  int socket_fd;
private:
  string recvMessage(size_t bytes); // reveive all message (unnecessary to be virtual)
  int parseBytes();
public:
  const int getSocket() const { return socket_fd; };
  Socket();
  Socket(int __socket_fd);
  Socket(const Socket & rhs) = delete; // copy constructor
  Socket & operator=(const Socket & rhs) = delete; // assignment operator
  Socket(Socket && rhs) noexcept;
  Socket& operator=(Socket && rhs) noexcept;
  bool isValid();
  virtual ~Socket();
};

class Listener : public Socket {
  // socket_fd
  const char * port;
public:
  Listener() = default;
  Listener(const char * _port);
  Listener(const Listener & rhs) = delete; // copy constructor
  Listener & operator=(const Listener & rhs) = delete; // assignment operator  
  Listener(Listener && rhs) noexcept;
  Listener & operator=(Listener && rhs) noexcept;
  Socket Accept();
  ~Listener();
};

class Connector : public Socket {
  const char * host;
  const char * port;
public:
  Connector() = default;
  Connector(const char * host, const char * port);
  Connector(const Connector & rhs) = delete; // copy constructor
  Connector & operator=(const Connector & rhs) = delete; // assignment operator
  Connector (Connector && rhs) noexcept;
  Connector & operator=(Connector && rhs) noexcept;
  ~Connector();
};


#endif
