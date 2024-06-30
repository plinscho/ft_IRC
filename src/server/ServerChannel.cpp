#include "Server.hpp"
#include <iostream>
#include <string>

void	Server::initChannels(void)
{
	std::vector<std::string>::iterator it;
	std::vector<std::string> channelNames;

	channelNames.push_back("#General");
	channelNames.push_back("#Music");
	channelNames.push_back("#Random");
	channelNames.push_back("#Gaming");
	channelNames.push_back("#Cotilleo");
	int i = 1;

	for (it = channelNames.begin() ; it != channelNames.end() ; ++it)
	{
		createChannel(i, it->c_str());
		i++;
	}
}

void	Server::createChannel(int id, const std::string channelName)
{
	Channel *newChannel;
	try
	{
		newChannel = new Channel(id, channelName);
	}
	catch (const std::bad_alloc& e)
	{
		std::cerr << "Failed to allocate memory for new Channel: " << e.what() << '\n';
		return ;
	}
	_channels[id] = newChannel;
}

void	Server::deleteChannel(int id)
{
	Channel *tmpChannel;
	std::map<int, Channel *>::iterator channelIterator;
	channelIterator = _channels.find(id);
	if (channelIterator != _channels.end())
	{
		tmpChannel = channelIterator->second;
		std::cout << tmpChannel->getChannelName() << " deleted.\n" << std::endl;

		// free memory
		delete tmpChannel;
		tmpChannel = NULL;
		_channels.erase(channelIterator);
	}
}

void	Server::addClientToChannel(Client *user, Channel *channel)
{
	std::string msg = "";

	if (channel->activeUsers >= MAX_CHANNEL_USERS)
		return ;
	channel->addUser(user->getFd(), user);
	msg = user->getNickname() + " joined " + channel->getChannelName() + " channel.\n";
	sendMessage(user, msg);	
}