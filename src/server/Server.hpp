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

#define TIMEOUT 10000
#define MAX_CLIENTS 10

class Server {
  private:
	bool						powerOn;
	int 						_port;
	int 						sockfd;
	sockaddr_in 				sockaddr;	// Server address
	std::vector <Client *>		clientVector; // Clients connected to the server
	std::vector <struct pollfd> pollVector;
	std::string 				p_port;
	std::string 				p_password;

  public:
	int							conectedClients;
	Server(int, char *);
	~Server();
	Client * getClientByFd(int fdMatch);
	bool	isPowerOn() const;
	void	shutDown();
	int		getPort() const;
	int		getSockfd() const;
	void	handleNewConnection();
	void	handleDisconnection(int index);
	void	initPoll();
	void	handleConns();
	void	run();
	void	closeSockets();
};
