#pragma once
#include "../client/Client.hpp"
#include "../messages/Messages.hpp"
#include "Channel.hpp"

#include <cerrno>
#include <iostream>     // For cout
#include <netinet/in.h> // For sockaddr_in
#include <stdlib.h>     // atoi
#include <sys/socket.h> // For socket functions
#include <unistd.h>     // For read
#include <poll.h>
#include <vector>
#include <map>
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <sys/types.h>
#include <algorithm> 

extern bool power;

#define POLL_TIMEOUT -1
#define MAX_CLIENTS 10

class Client;

class Server 
{
  private:

	int 							_port;
	int 							_sockfd;
	std::string 					_password;
	sockaddr_in 					_sockaddr;	// Server address
	std::vector <struct pollfd> 	_vectorPoll;
    std::map<int, Client*> 			_fdToClientMap; // Map file descriptors to Client pointers
	std::map<int, Channel*>			_channels;

  public:

	Server(int, char *);
	~Server();
	Messages message;
	int								conectedClients;
	char							buffer[MAX_MSG_SIZE];

	void handshake(Client *user);

//	CHANNEL MANAGING
	void							initChannels();
	void							createChannel(int id, const std::string channelName);
	void							deleteChannel(int id);
	void							addClientToChannel(Client *user, Channel *channel);

	int								updatePoll();
	void							handleWriteEvent(int fd);
	int								grabConnection();
	int								run();
	void							receiveData(int fd);
	void							closeServer();
	void							initPoll();
	void							handleDisconnection(int index);
	int								getPort() const;
	std::string						getPassword() const;
	int								getSockfd() const;
	std::vector<pollfd>::iterator	findPollFd(int fd);
	int								handleInput(Client *user);

	// COMMANDS
	int 							cmdLogin(std::vector<std::string> cmd, Client *user);
	int 							cmdJoin(std::vector<std::string> cmd, Client *user);
	int 							cmdSetNick(std::vector<std::string> cmd, Client *user);
	int 							cmdSetUname(std::vector<std::string> cmd, Client *user);
	int 							cmdSend(std::vector<std::string> cmd, Client *user);
	int								cmdChannel(std::vector<std::string> cmd, Client *user);
	int 							cmdHelp(std::vector<std::string> cmd, Client *user);
};

// COMMANDS

enum cmdType
{
	CMD_LOGIN = 0,
	CMD_JOIN,
	CMD_SETNICK,
	CMD_SETUNAME,
	CMD_SEND,
	CMD_HELP,
	SEND_MSG
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

std::string 				trim(const std::string& str);
int							preCmdCheck(std::vector<std::string> cmd, Client *user);
int							sendMessage(Client *user, const std::string &msg);
int							quickError(std::string msg, int errcode);
int							checkNick(std::string newNick);
int							setNick(int type, Client *user, std::string newNick);
int 						sendWelcome(int fd);
cmdType 					getCommandType(const std::string &cmd);
std::vector<std::string> 	stringSplit(const char *str, const char& c);



