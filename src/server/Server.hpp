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
	std::map<std::string, Client*> 	_nicknameMap; // Here we will store the nicknames
	std::map<int, Channel*>			_channels;

  public:

	Server(int, char *);
	~Server();
	Messages 						message;
	int								conectedClients;
	char							buffer[MAX_MSG_SIZE];

	int								run();
	void							closeServer();
	
//	CHANNEL MANAGING
	void							initChannels();
	void							createChannel(int id, const std::string channelName);
	void							deleteChannel(int id);
	void							addClientToChannel(Client *user, Channel *channel);

//	CONNECTIVITY
	void							initPoll();
	int								updatePoll();
	int								grabConnection();
	void							handleDisconnection(int index);
	void							sendWelcome(Client *user);

//	DATA
	int								handleInput(std::string cmd, int fd);
	void							sendData(pollfd &pollfdStruct);
	void							receiveData(pollfd &pollfdStruct);

//	GETERS & FINDERS
	int								getPort() const;
	int								getSockfd() const;
	std::string						getPassword() const;
	std::vector<pollfd>::iterator	findPollFd(int fd);

//	COMMANDS
	int								setPass(Client *user, std::string command, std::string pass);
	int								setNick(Client *user, std::string command, std::string nick);
	bool 							cmdNick(Client* user, std::string newNick);
	int								setUser(Client *user, std::string command, std::string newUser);

//	NICK functions
	bool 							isNicknameInUse(const std::string &nickname) const;
	void 							registerNickname(const std::string &nickname, Client* newUser);
	void 							unregisterNickname(const std::string &nickname);
};

enum cmdType
{
	CMD_CAP = 0,
	CMD_QUIT,
	CMD_PASS,
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
bool						getLogStat(Client *user);
std::string 				stringToHex(const std::string& str);
bool						toggleBool(bool state);
int							sendMessage(Client *user, const std::string &msg);
int							quickError(std::string msg, int errcode);
nickReturn					checkNick(std::string newNick);
int							switchNick(int type, Client *user, std::string newNick);
cmdType 					getCommandType(const std::string &cmd);
std::vector<std::string> 	stringSplit(std::string str, char c);
std::vector<std::string>	stringSplit(std::string str, std::string delimiter);
std::vector<std::string>	stringTrimSplit(std::string str, std::string delimiter);




