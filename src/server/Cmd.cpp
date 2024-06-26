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
    if (cmd.empty() || !user || user->_loginAtempts >= 3) 
	{
		send(user->getFd(), "Logging failed.\nDisconnecting user...\n", 39, MSG_DONTWAIT);
		return (-1);
	}

	int attempts = 3 - user->_loginAtempts; 
	std::stringstream ss;
	ss << "Incorrect password.\n" << attempts << " attemps left.\n";
	std::string errpsw = ss.str();
	
	if (cmd[1] != _password)
	{
		if (send(user->getFd(), errpsw.c_str(), errpsw.length(), MSG_DONTWAIT) == -1)
			return (-1);
		user->_loginAtempts++;
		return (0);
	}

	int nick = checkNick(cmd[2]);
	switch (nick)
	{
		case NICK_OK:
		{
			user->setNickname(cmd[2]);
			if (send(user->getFd(), "Nickname succesfully changed\n", 30, MSG_DONTWAIT) == -1)
				return (-1);
			break;
		}
		case EMPTY_NICK:
		{
			if (send(user->getFd(), "Error. Empty nick is not allowed\n", 34, MSG_DONTWAIT) == -1)
				return (-1);
			break;
		}
		case SIZE_EXCEED:
		{
			if (send(user->getFd(), "Error. Nick is more than 8 chars\n", 34, MSG_DONTWAIT) == -1)
				return (-1);
			break;
		}
		case HAS_SPACE:
		{
			if (send(user->getFd(), "Error. Space chars in nick are not allowed\n", 44, MSG_DONTWAIT) == -1)
				return (-1);
			break;
		}
		case IS_NOT_ALNUM:
		{
			if (send(user->getFd(), "Error. Non alnum chars in nick detected\n", 41, MSG_DONTWAIT) == -1)
				return (-1);
			break;
		}
	}
	return (0);
}

int Server::cmdJoin(std::vector<std::string> cmd, Client *user)
{
    if (cmd.empty() || !user)
        return (-1);
    return (0);
}

int Server::cmdSetNick(std::vector<std::string> cmd, Client *user)
{
    if (cmd.empty() || !user)
        return (-1);

    return (0);
}

int Server::cmdSetUname(std::vector<std::string> cmd, Client *user)
{
    if (cmd.empty() || !user)
        return (-1);

    return (0);
}

int Server::cmdSend(std::vector<std::string> cmd, Client *user)
{
    if (cmd.empty() || !user)
        return (-1);

    return (0);
}

int Server::cmdHelp(std::vector<std::string> cmd, Client *user)
{
    if (cmd.empty() || !user)
        return (-1);
    if (user->getFd() == -1)
        return (-1);
    return (sendWelcome(user->getFd()));
}
