#include "Server.hpp"
#include <iostream>
#include <string>


void	Server::createChannel(const std::string channelName)
{
	Channel *newChannel;
	try
	{
		newChannel = new Channel(channelName);
	}
	catch (const std::bad_alloc& e)
	{
		std::cerr << "Failed to allocate memory for new Channel: " << e.what() << '\n';
		return ;
	}
	_channels[channelName] = newChannel;
}

void Server::deleteChannel(std::string channelName) 
{
    std::map<std::string, Channel*>::iterator channelIterator;

    for (channelIterator = _channels.begin(); channelIterator != _channels.end(); ++channelIterator)
	{
        if (channelIterator->second->getChannelName() == channelName)
		{
            std::cout << channelIterator->second->getChannelName() << " deleted.\n" << std::endl;
            delete channelIterator->second; // Liberar memoria
            _channels.erase(channelIterator); // Eliminar el canal del mapa usando el iterador
            return; // Salir de la función después de borrar el canal
        }
    }
}

void	Server::addClientToChannel(Client &user, Channel &channel)
{
	std::string msg = "";

	if (channel.activeUsers >= channel.maxUsers)
		return ;
	channel.addUser(user.getFd(), user);
	msg = "JOIN " + channel.getChannelName() + "\r\n";
	message.sendMessage(user, msg);	
}