#include "Channel.hpp"
#include "../client/Client.hpp"
#include <iostream>
#include <algorithm>
#include <string>
#include <cstdio>


Channel::Channel(int id, const std::string channelName)
{
	this->_channelId = id;
	this->_channelName = channelName;
	this->activeUsers = 0;
	this->_topic = "";   
    this->_mode.setMode("t");
	this->maxUsers = __INT_MAX__;
}

Channel::~Channel()
{

}

// Devuleve el NAMES para el canal
std::vector<std::string> Channel::getChannelsNicks()
{
    Client* currentClient;
    std::string completeNick;
    std::map<int, Client*>::iterator it;
    std::vector<std::string> nicks;

    // Primero, agregar todos los nicks del mapa _fdUsersMap a nicks
    for (it = _fdUsersMap.begin(); it != _fdUsersMap.end(); ++it)
    {
        currentClient = it->second;
        nicks.push_back(currentClient->getNickname());
    }

    // Luego, comparar cada nick con los nicks en nickOp
    for (size_t i = 0; i < nicks.size(); ++i)
    {
        if (std::find(nickOp.begin(), nickOp.end(), nicks[i]) != nickOp.end())
        {
            // Si encuentra un match, reemplazar por el mismo nick pero con un "@" delante
            nicks[i] = "@" + nicks[i];
        }
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

void Channel::broadcastMessage(const std::string &msg) 
{
	std::map<int, Client *>::iterator it;
    for (it = _fdUsersMap.begin(); it != _fdUsersMap.end(); ++it) {
        message.sendMessage(*it->second, msg);
    }
}

void Channel::broadcastMessageExcludeSender(Client *who, const std::string &msg) 
{
	std::map<int, Client *>::iterator it;
    for (it = _fdUsersMap.begin(); it != _fdUsersMap.end(); ++it) {
        if (who->getNickname() == it->second->getNickname())
            continue;
        message.sendMessage(*it->second, msg);
    }
}

bool Channel::isUserOp(std::string nickInChannel)
{
	std::vector<std::string>::iterator it;

	for (it = nickOp.begin() ; it != nickOp.end() ; ++it)
	{
		if (*it == nickInChannel)
			return (true);
	}
	return (false);
}

int Channel::setNewId()
{
	static int id = 0;
	return id++;
}

void	Channel::addUser(int fd, Client &newUser)
{
	_fdUsersMap[fd] = &newUser;
	activeUsers++;
}

void    Channel::removeUser(int fd)
{
    _fdUsersMap.erase(fd);
    activeUsers--;
}