#pragma once
#include <cerrno>
#include <cstdlib>      // For exit() and EXIT_FAILURE
#include <iostream>     // For cout
#include <netinet/in.h> // For sockaddr_in
#include <sys/socket.h> // For socket functions
#include <unistd.h>     // For read
#include <arpa/inet.h>  // For inet_ntoa

class Client {
  private:
	int 				clientSocketFd;
	struct sockaddr_in	clientAddr;

  public:
    Client(int fd, struct sockaddr_in address);
    ~Client();
	std::string		getAddress() const ;
	int				getFd() const ;
	size_t			receiveData(char *buffer, size_t msgLenght);
	size_t			sendData(const char *buffer, size_t msgLenght);


};