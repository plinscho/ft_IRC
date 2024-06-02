#pragma once
#include "../client/Client.hpp"
#include <cerrno>
#include <cstdlib>      // For exit() and EXIT_FAILURE
#include <iostream>     // For cout
#include <netinet/in.h> // For sockaddr_in
#include <stdlib.h>     // atoi
#include <sys/socket.h> // For socket functions
#include <unistd.h>     // For read
#include <poll.h>
#include <vector>

#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 1024

class Server {
  private:
    int _port;
    int sockfd;
    //	std::vector<User *>		connectedUsers;
    //	std::vector<struct pollfd>	userFDs;
    sockaddr_in sockaddr;

    std::string p_port;
    std::string p_password;

  public:
	pollfd 		users[MAX_CLIENTS];
    Server(int, char *);
    ~Server();
    void ListenAndServe();
    void createSocket();
    void bindSocket();
    void listenSockets();
    void handleConns();
	void getConnections();
};
