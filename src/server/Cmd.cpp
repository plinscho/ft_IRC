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
