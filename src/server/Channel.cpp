#include "Channel.hpp"
#include <iostream>
#include <string>

Channel::Channel()
: _channelId(-1), _channelName("") // Initializer list
{

}

Channel::Channel(int id, const std::string channelName)
{
	this->_channelId = id;
	this->_channelName = channelName;
	this->activeUsers = 0;
}

Channel::~Channel()
{

}

std::string Channel::getChannelName(void)  
{
	return _channelName;
}

int			Channel::getChannelId(void) 
{
	return _channelId;
}

void	Channel::addUser(int fd, Client *newUser)
{
	_fdUsersMap[fd] = newUser;
	activeUsers++;
}