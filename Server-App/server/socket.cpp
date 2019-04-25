#include "socket.hpp"

Socket::Socket() : socket_fd(-1) {}

Socket::Socket(int __socket_fd) : socket_fd(__socket_fd) {}

Socket::Socket(Socket && rhs) noexcept {
  socket_fd = exchange(rhs.socket_fd, -1); // avoid double closing socket
}

Socket& Socket::operator=(Socket && rhs) noexcept {
  if (this != &rhs) {
    std::swap(socket_fd, rhs.socket_fd);
  }
  return * this;
}


bool Socket::isValid() {
  return socket_fd > -1;
}


Socket::~Socket() {
  if (socket_fd > -1) {
    cout << "closing socket " << this->socket_fd << endl;
    auto status = TEMP_FAILURE_RETRY(close(socket_fd));
    if (status == -1) {
      perror("Close Socket Failed");
    } 
  }
}

Listener::Listener(const char * _port) : Socket(), port(_port) {
  int status;
  struct addrinfo host_info;
  struct addrinfo * host_info_list;
  const char * hostname = NULL;

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags    = AI_PASSIVE;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);

    
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    // throw exception
    
  }

  socket_fd = socket(host_info_list->ai_family,
		     host_info_list->ai_socktype,
		     host_info_list->ai_protocol);

  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    // throw exception    
  }

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    // throw exception
  }

  status = listen(socket_fd, 100);
  if (status == -1) {
    cerr << "Error: cannot elisten on socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    // throw exception
  }
  freeaddrinfo(host_info_list);
}

Listener::~Listener(){}

Socket Listener::Accept() {
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  int client_connection_fd;
  // throw 7;
  client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  // cout << client_connection_fd << endl;
  if (client_connection_fd == -1) {
    cerr << "Error: cannot accept connection on socket" << endl;
    // throw exception
  }
  return move(Socket(client_connection_fd));
}

Listener::Listener(Listener && rhs) noexcept {
  Socket(move(rhs));
  swap(this->port, rhs.port);
}

Listener & Listener::operator=(Listener && rhs) noexcept {
  if (this != &rhs) {
    Socket::operator=(std::move(rhs));
    swap(this->port, rhs.port);
  }
  return *this;
}

Connector::Connector(const char * _host, const char * _port) : Socket(), host(_host), port(_port) {
  int status;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(host, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << host << "," << port << ")" << endl;
    // throw exception
  } 

  socket_fd = socket(host_info_list->ai_family,
		     host_info_list->ai_socktype,
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << host << "," << port << ")" << endl;
    // throw exception
  } 

  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << host << "," << port << ")" << endl;
    // throw excpetion
  } 
  cout << "Socket_fd int Conntector() " << socket_fd << endl;
  freeaddrinfo(host_info_list);
}

Connector::Connector(Connector && rhs) noexcept {
  Socket(move(rhs));
  // port = exchange(rhs.port, nullptr);
  // host = exchange(rhs.host, nullptr);
  swap(port, rhs.port);
  swap(host, rhs.host);
} 
 
Connector & Connector::operator=(Connector && rhs) noexcept {
  // cout << rhs.socket_fd << endl;
  if(this != &rhs) {
    Socket::operator=(move(rhs));
    swap(port, rhs.port);
    swap(host, rhs.host);
  }
  return * this;
}

Connector::~Connector() {}







		       




		  
		  

		     

