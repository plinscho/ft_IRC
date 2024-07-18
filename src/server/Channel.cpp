#include "Channel.hpp"
#include "../client/Client.hpp"
#include <iostream>
#include <string>
#include <cstdio>


Channel::Channel()
: _channelId(-1), _channelName("") // Initializer list
{

}

Channel::Channel(int id, const std::string channelName)
{
	this->_channelId = id;
	this->_channelName = channelName;
	this->activeUsers = 0;
	this->_topic = "";
}

Channel::~Channel()
{

}

std::vector<std::string> Channel::getChannelsNicks()
{
    Client* currentClient;
    std::map<int, Client*>::iterator it;
    std::vector<std::string> nicks;

    if (_fdUsersMap.empty())
        return nicks;

    for (it = _fdUsersMap.begin(); it != _fdUsersMap.end(); ++it)
    {
        currentClient = it->second;
        nicks.push_back(currentClient->getNickname());
        printf("Nick: %s\n", currentClient->getNickname().c_str());
    }
    return nicks;
}

std::string Channel::getChannelName(void)  
{
	return _channelName;
}

int			Channel::getChannelId(void) 
{
	return _channelId;
}

std::string	Channel::getTopic(void)
{
	return (this->_topic);
}

void	Channel::setTopic(std::string &topic)
{
	this->_topic = topic;
}

void Channel::broadcastMessage(const std::string &message) 
{
	std::map<int, Client *>::iterator it;
    for (it = _fdUsersMap.begin(); it != _fdUsersMap.end(); ++it) {
        sendMessage(it->second, message);
    }
}

int Channel::setNewId()
{
	static int id = 0;
	return id++;
}

void	Channel::addUser(int fd, Client *newUser)
{
	_fdUsersMap[fd] = newUser;
	activeUsers++;
}