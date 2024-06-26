#pragma once
#include <string.h>
#include <cerrno>
#include <cstdlib>      // For exit() and EXIT_FAILURE
#include <poll.h>
#include <iostream>     // For cout
#include <netinet/in.h> // For sockaddr_in
#include <sys/socket.h> // For socket functions
#include <unistd.h>     // For read
#include <arpa/inet.h>  // For inet_ntoa

#define MAX_MSG_SIZE 1024

class Client {
  private:
	char				_recvBuffer[MAX_MSG_SIZE];
	char				_sendBuffer[MAX_MSG_SIZE];
	int 				_clientSocketFd;
	std::string			_clientIp;
	std::string			_nickName;
	std::string			_userName;

	bool				_registered;
	bool				_logged;


  public:
	Client();
    Client(int fd, std::string ip);
    ~Client();
	std::string		getNickname() const ;
	std::string		getAddress() const ;
	std::string		getRecvBuffer() const ;
	int				getFd() const ;

	void			setNickname(std::string newNick);

	int				sendData(int serverFd);
	int				receiveData(int serverFd);
};

enum nickReturn
{
	NICK_OK = 0,
	EMPTY_NICK,
	SIZE_EXCEED,
	HAS_SPACE,
	IS_NOT_ALNUM,	
};

