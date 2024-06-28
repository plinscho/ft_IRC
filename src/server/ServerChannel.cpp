#include "Server.hpp"
#include <iostream>
#include <string>

void	Server::createChannel(int id, const std::string channelName)
{
	(void) id;
	(void) (channelName);
	
	Channel *newChannel;
	try
	{
		newChannel = new Channel(id, channelName);
	}
	catch (const std::bad_alloc& e)
	{
		std::cerr << "Failed to allocate memory for new Channel: " << e.what() << '\n';
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
	if (channel->activeUsers >= MAX_CHANNEL_USERS)
		return ;
	channel->addUser(user->getFd(), user);

}