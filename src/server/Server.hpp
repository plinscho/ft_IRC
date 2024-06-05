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

extern bool power;

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
	char						buffer[MAX_MSG_SIZE];
	Server(int, char *);
	~Server();
	Client * getClientByFd(int fdMatch);
	void	handleCmd(const char *buffer, Client *clientObj);
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


/*

send() flags:

MSG_CONFIRM: 	This flag is not relevant for send(). It's used in sendto() function to tell the kernel that more data is coming in a subsequent call to sendto().
MSG_DONTROUTE: 	This flag requests that the data should not be subject to routing. The data should be sent only to hosts on directly connected networks.
MSG_DONTWAIT: 	This flag specifies that the operation should be performed in non-blocking mode. If the operation would block, send() will fail with the error EAGAIN or EWOULDBLOCK.
MSG_EOR: 		This flag indicates end-of-record for record-based sockets (SOCK_SEQPACKET and SOCK_RDM).
MSG_MORE: 		This flag indicates that more data is coming. The data will be bundled together into a single TCP segment if possible.
MSG_NOSIGNAL: 	This flag requests not to send the SIGPIPE signal if an attempt to send is made on a stream socket that is no longer connected.
MSG_OOB: 		This flag sends out-of-band data on sockets that support this notion.
*/
void	sendMsgFd(int destFd, std::string msg, int flag);
int		recvMsgFd(int originFd, char *buffer, size_t maxLen, int flag);
int		checkNick(std::string newNick);

enum nickReturn
{
	NICK_OK = 0,
	EMPTY_NICK,
	SIZE_EXCEED,
	HAS_SPACE,
	IS_NOT_ALNUM,	
};