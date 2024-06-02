#pragma once
#include <cerrno>
#include <vector>
#include <cstdlib>      // For exit() and EXIT_FAILURE
#include <iostream>     // For cout
#include <netinet/in.h> // For sockaddr_in
#include <sys/socket.h> // For socket functions
#include <unistd.h>     // For read
#include <stdlib.h>		// atoi
#include "../client/Client.hpp"


class Server 
{
	private:
	int port;
	int sockfd;
//	std::vector<User *>		connectedUsers;	
//	std::vector<struct pollfd>	userFDs;
	sockaddr_in	sockaddr;
    std::string p_port;
    std::string p_password;
  public:
    Server(int, const char *);
    ~Server();
    void ListenAndServe();
    void createSocket();
    void bindSocket();
    void listenSockets();
    void handleConns();
};
