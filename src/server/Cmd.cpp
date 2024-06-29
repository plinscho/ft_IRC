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

int Server::cmdLogin(std::vector<std::string> cmd, Client *user)
{	
	if (user->_loginAtempts >= 3){
		return (sendMessage(user, "Logging failed.\nDisconnecting user...\n"));
	} else if (cmd.empty() || !user) {
		return (-1);
	}

	int attempts = 3 - user->_loginAtempts; 
	std::stringstream ss;
	ss << "Incorrect password.\n" << attempts << " attemps left.\n";
	std::string errpsw = ss.str();
	
	if (cmd.size() != 3 || cmd[1].empty() || cmd[2].empty())
		return (sendMessage(user, "Usage: /login <password> <nickname>\n"));
	if (cmd[1] != _password)
	{
		user->_loginAtempts++;
		return (sendMessage(user, errpsw));
	}

	int type = checkNick(cmd[2]);
	if (setNick(type, user, cmd[2]) == -1)
	{
		handleDisconnection(user->getFd());
		return (0);
	}
	user->setLogin(true);
	std::stringstream ss2;
	ss2 << "User " << user->getNickname() << " logged in .\n";
	std::string logedIn = ss2.str();
	return (sendMessage(user, logedIn));
}

int Server::cmdJoin(std::vector<std::string> cmd, Client *user)
{
	int ret;

	ret = preCmdCheck(cmd, user);
	if (ret == -1)
		return (-1);
	else if (ret != 0)
		return (1);
	

	return (0);
}

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
	
	return (sendWelcome(user->getFd()));
}
