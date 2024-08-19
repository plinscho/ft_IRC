#pragma once
#include <string.h>
#include <vector>
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

	int 				_clientSocketFd;
	std::string			_clientIp;
	std::string			_nickName;
	std::string			_userName;
	std::string			_clientBuffer;
	bool				_logged;
	bool				_hasNick;
	bool				_hasPass;
	bool				_hasUser;

  public:
	std::vector<std::string> 	channelsJoined;
	std::vector<std::string>	handshakeVector;
	bool						changeRevent;
	
	Client();
    Client(int fd, std::string ip);
    ~Client();
	std::string		getNickname() const;
	std::string		getUsername() const;
	std::string		getAddress() const;
	std::string		getRecvBuffer() const;
	int				getFd() const;
	bool			getLogin(void) const;
	bool			getLogStat();

	void			eraseJoinedChannel(std::string channelName);
	void			clearBuffer(void) {this->_clientBuffer.clear();}
	void			setBuffer(std::string newBuffer);
	void			setNickname(std::string newNick);
	void			setUsername(std::string newUsername);
	void			setLogin(bool option);
	void			setHasPass(bool option) {this->_hasPass = option;}
	void			setHasNick(bool option) {this->_hasNick = option;}
	void			setHasUser(bool option) {this->_hasUser = option;}

	bool			getHasPas(){return (_hasPass);}
	bool			getHasUser(){return (_hasUser);}
	bool			getHasNick(){return (_hasNick);}
	std::string 	getPrefix() const;
};

enum nickReturn
{
	NICK_OK = 0,
	EMPTY_NICK,
	SIZE_EXCEED,
	HAS_SPACE,
	IS_NOT_ALNUM,	
};

// if true, perform handshake
bool	checkHandshakeOptions(Client *user);
void	fillHandshakeVector(Client *user, std::string &clientHandshakeBuffer);