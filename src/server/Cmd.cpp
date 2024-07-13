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

int Server::cmdLogin(std::vector<std::string> lines, Client *user)
{
	sendMessage(user, "Logging in ...\nChecking password.\n");
	std::string msg = "";
	std::vector<std::string>::iterator it;
	std::vector<std::string> cmd;
	static bool passwordOk = false, nickSet = false, userSet = false;

	int i = 0;
	for (it = lines.begin() ; it != lines.end() ; ++it, ++i)
	{
		std::stringstream ss;
		std::cout << "line: " << i << " = " << *it << std::endl;
		cmd = stringSplit(it->c_str(), ' ');
		if (cmd[0] == "PASS"){
			if (cmd.size() < 2 || cmd[1] != _password){
				passwordOk = true;
			} else {
				sendMessage(user, "Incorrect password.\nDisconnecting.\n");
				return (-1);
			}
		}
		else if (cmd[0] == "NICK"){
			user->setNickname(cmd[1]);
			nickSet = true;
			ss << "Setting nickname to: " << cmd[1] << std::endl;
		}
		else if (cmd[0] == "USER"){
			user->setUsername(cmd[1]);
			ss << "Setting username to: " << cmd[1] << std::endl;
			userSet = true;
		}
		std::string msg = ss.str();
		sendMessage(user, msg);
		msg.clear();
		cmd.clear();
	}
	if (passwordOk && nickSet && userSet)
    {
        user->setLogin(true);
        std::string loggedIn = "User logged in.\n";
        return sendMessage(user, loggedIn);
    }
    else
    {
        sendMessage(user, "Login failed. Missing information.\n");
        return (-1);
    }
}

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
int	Server::checkPass(Client *user, std::string command, std::string pass)
{
	std::string response;

	if (command.empty() || pass.empty())
	{
	// Construir el mensaje de vuelta
		response = message.getMessages(461, *user);
		sendMessage(user, response);
		return (1);
	}

//		std::cout << getPassword().length() << pass.length() << std::endl;
	if (getPassword() == pass)
	{
		user->setHasPass(true);
		return (0);
	}
	return (1);
}

int	Server::checkNick(Client *user, std::string command, std::string nick)
{
	std::string response;
	if (command.empty() || nick.empty())
	{
	// Construir el mensaje de vuelta
		response = message.getMessages(461, *user);
		sendMessage(user, response);
		return (0);
	}
	// If else struct for client handshake
	if (!user->getLogin() && !isNicknameInUse(nick))
	{
		registerNickname(nick, user);
		user->setHasNick(true);
	}
	else
	{
		response = message.getMessages(433, *user);
		sendMessage(user, response);
		// returning to disconnect (handleDisconnection after switch case if 1 is returned)
		return (1);
	}
	if (user->getLogin())
	{
	 	if(isNicknameInUse(nick))
		{
			response = message.getMessages(433, *user);
			sendMessage(user, response);
			return (0);
		}
		response = message.getMessages(1001, *user);
		response += nick + "\r\n";
		unregisterNickname(user->getNickname());
		registerNickname(nick, user);
		sendMessage(user, response);
		return (0);
	}

	return (0);
}

int	Server::checkUser(Client *user, std::string command, std::string newUser)
{
	std::string response;
	if (command.empty() || newUser.empty())
	{
	// Construir el mensaje de vuelta
		response = message.getMessages(461, *user);
		sendMessage(user, response);
		return (0);
	}
	user->setUsername(newUser);
	user->setHasUser(true);
	return (0);
}

int Server::cmdJoin(std::vector<std::string> cmd, Client *user)
{
	Channel *tmpChannel;
	std::map<int, Channel *>::iterator channelIt;
	std::string channelName = trim(cmd[1]);

	std::cout << "join mandado\n"<< std::endl;
//	std::cout << "cmd[1]: " << cmd[1] << std::endl;

	std::cout << "Channel selected: " << channelName << std::endl;
	if (cmd.size() != 2)
		return (sendMessage(user, "Usage: /join <channel name>\n"));
	for (channelIt = _channels.begin() ; channelIt != _channels.end() ; ++channelIt)
	{
		std::cout << channelIt->second->getChannelName() << std::endl;
		if (channelIt->second->getChannelName() == channelName)
		{
			tmpChannel = channelIt->second;
			addClientToChannel(user, tmpChannel);
			sendMessage(user, "PRIVMSG #General\r\n");
			return (0);
		}
	}
	sendMessage(user, "channel not found.\n");
	return (1);
}

/*
int Server::cmdSetNick(std::vector<std::string> cmd, Client *user)
{
	int type;
	int ret;

	ret = preCmdCheck(cmd, user);
	if (ret == -1)
		return (-1);
	else if (ret != 0)
		return (1);
	else if (cmd.size() != 2 || cmd[1].empty())
		return (sendMessage(user, "Usage: /setnick <newnick>\n"));
	type = checkNick(cmd[1]);
	if (setNick(type, user, cmd[1]) == -1)
		return (-1);
	return (0);
}
*/

int Server::cmdSetUname(std::vector<std::string> cmd, Client *user)
{
	int ret;

	ret = preCmdCheck(cmd, user);
	sendMessage(user, "Hola esto es un mensaje de vuelta\n");
	if (ret == -1)
		return (-1);
	else if (ret != 0)
		return (1);

	return (0);
}

int Server::cmdSend(std::vector<std::string> cmd, Client *user)
{
	int ret;

	ret = preCmdCheck(cmd, user);
	if (ret == -1)
		return (-1);
	else if (ret != 0)
		return (1);
	

	return (0);
}

int	Server::cmdChannel(std::vector<std::string> cmd, Client *user)
{
	int ret;

	ret = preCmdCheck(cmd, user);
	if (ret == -1)
		return (-1);
	else if (ret != 0)
		return (1);

	std::map<int, Channel *>::iterator mapIt;
	for (mapIt = _channels.begin() ; mapIt != _channels.end() ; ++mapIt)
	{
		sendMessage(user, mapIt->second->getChannelName());
	}
	return (0);
}

int Server::cmdHelp(std::vector<std::string> cmd, Client *user)
{
	int ret;

	ret = preCmdCheck(cmd, user);
	if (ret == -1)
		return (-1);
	else if (ret != 0)
		return (1);
	return (0);
}
