#include "../client/Client.hpp"
#include "Server.hpp"
#include <iostream>
#include <vector>
#include <sstream>

/*
	All ow these commands will return to handleInput() function.
	If something is wrong with a function, we can return -1 and 
	proceed to disconnect the client form the server.
*/


/*

4.1.1 Mensaje de Password


	  Comando: PASS
   Parámetros: <password>

   El comando PASS se usa para establecer una "clave de conexión". La
   clave puede y debe establecerse antes de cualquier intento de
   realizar la conexión. Esto requiere que los clientes envíen el
   comando PASS antes de la combinación NICK/USUARIO, y los servidores
   *deben* enviar el comando PASS antes de cualquier comando SERVER. La
   clave debe coincidir con una de las líneas C/N (para servidores) o
   las I (para clientes). Es posible enviar múltiples comandos PASS
   antes del registro pero sólo la última que se envía se verifica y no
   puede cambiarse una vez hecho el registro. Respuestas numéricas:

		   ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED
*/


int	Server::setUser(Client *user, std::string command, std::string newUser)
{
	std::string response;
	if (command.empty() || newUser.empty())
	{
	// Construir el mensaje de vuelta
		response = message.getMessages(461, *user);
		message.sendMessage(user, response);
		return (1);
	}
	user->setUsername(newUser);
	user->setHasUser(true);
	response = "Username set to " + newUser + "\r\n";
	message.sendMessage(user, response);
	return (0);
}


int	Server::setPass(Client *user, std::string command, std::string pass)
{
	std::string response;

	if (command.empty() || pass.empty())
	{
	// Construir el mensaje de vuelta
		response = message.getMessages(461, *user);
		message.sendMessage(user, response);
		return (1);
	}

//		std::cout << getPassword().length() << pass.length() << std::endl;
	if (getPassword() == pass)
	{
		response = "Password accepted.\r\n";
		message.sendMessage(user, response);
		user->setHasPass(true);
		return (0);
	}
	return (1);
}

nickReturn	checkNick(std::string& newNick)
{
	if (newNick.empty()) return EMPTY_NICK;
	if (newNick.size() > 8) return SIZE_EXCEED;
	if (newNick.find(' ') != std::string::npos)	return HAS_SPACE;
	for (ssize_t i = 0 ; newNick[i] != '\0'; ++i)
	{
		unsigned char c = newNick[i];
		if (!isalnum(c))
			return IS_NOT_ALNUM;
	}
	return NICK_OK;
}

// Handles the result of nickname validation and sets the nickname if valid
int Server::cmdNick(Client* user, std::string& newNick)
{
	nickReturn validationResult = checkNick(newNick);
	switch (validationResult) 
	{
		case NICK_OK:
			if (!user->getLogin() && !isNicknameInUse(newNick)) 
			{
				registerNickname(newNick, user);
				user->setNickname(newNick);
				user->setHasNick(true);
				message.sendMessage(user, "Nickname set to " + newNick + "\r\n");
				return NICK_OK;
			} 
			else if (isNicknameInUse(newNick)) {
				message.sendMessage(user, " Error. " + newNick + " is already in use.\r\n");
			} 
			else 
			{
				unregisterNickname(user->getNickname());
				registerNickname(newNick, user);
				message.sendMessage(user, "Nickname changed to " + newNick + "\r\n");
				return NICK_OK;
			}
			break;
		case EMPTY_NICK:
			message.sendMessage(user, "Error. Empty nick is not allowed.\r\n");
			break;
		case SIZE_EXCEED:
			message.sendMessage(user, "Error. Nick is more than 8 chars.\r\n");
			break;
		case HAS_SPACE:
			message.sendMessage(user, "Error. Space chars in nick are not allowed.\r\n");
			break;
		case IS_NOT_ALNUM:
			message.sendMessage(user, "Error. Non alnum chars in nick detected.\r\n");
			break;
	}
	message.sendMessage(user, "Try another nickname with /nick\r\n");
	return (validationResult);
}

void Server::sendChannelNames(Channel &channel, Client *user)
{
    std::string response;
    std::vector<std::string> channels = channel.getChannelsNicks();
    std::vector<std::string>::iterator it;

    // Formato del mensaje RPL_NAMREPLY
    response = ":irc.middleman.org 353 " + user->getNickname() + " = " + channel.getChannelName() + " :";
    for (it = channels.begin(); it != channels.end(); ++it)
    {
        response += *it + " ";
    }
    response += "\r\n";
    message.sendMessage(user, response);

    // Enviar RPL_ENDOFNAMES
    response = ":irc.middleman.org 366 " + user->getNickname() + " " + channel.getChannelName() + " :End of NAMES list\r\n";
    message.sendMessage(user, response);
}

int Server::cmdJoin(Client *user, std::string &channelName)
{
    std::string response;
    std::map<int, Channel *>::iterator it;

    if (channelName.empty())
    {
        response = message.getMessages(461, *user);
        message.sendMessage(user, response);
        return (0);
    }

    it = _channels.begin();
    while (it != _channels.end())
    {
        if (it->second->getChannelName() == channelName)
        {
            it->second->addUser(user->getFd(), user);

            // Notificar a todos en el canal sobre el nuevo usuario
            response = ":" + user->getPrefix() + " JOIN " + channelName + "\r\n";
            it->second->broadcastMessage(response);

            // Enviar el tema del canal si tiene uno
            std::string topic = it->second->getTopic();
            if (!topic.empty())
            {
                response = ":irc.middleman.org 332 " + user->getNickname() + " " + channelName + " :" + topic + "\r\n";
                message.sendMessage(user, response);
            }

            sendChannelNames(*it->second, user);
            return (0);
        }
        ++it;
    }

    if (channelName[0] != '#')
    {
        response = "Error. Channel name must start with #\r\n";
        message.sendMessage(user, response);
        return (0);
    }

    Channel *newChannel = new Channel(1, channelName);
    _channels[user->getFd()] = newChannel;
    newChannel->addUser(user->getFd(), user);

    // Notificar a todos en el canal sobre el nuevo usuario
    response = ":" + user->getPrefix() + " JOIN " + channelName + "\r\n";
    newChannel->broadcastMessage(response);

    sendChannelNames(*newChannel, user);
    return (0);
}



