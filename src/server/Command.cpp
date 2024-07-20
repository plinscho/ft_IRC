#include "Command.hpp"
#include "Server.hpp"
#include <iostream>

void    Command::getFromClientBuffer(const Client &user)
{
	//load buffer from client
	stringCommands = user.getRecvBuffer();
}

// partir esta funcion en varias
int Command::execute(Client &user, Server &server)
{
	// Parsear el comando
	std::vector<std::string> cmdSplitted;
	cmdSplitted = strTool.stringSplit(this->stringCommands, "\r\n");

	std::vector<std::string>::iterator it;
	for (it = cmdSplitted.begin() ; it != cmdSplitted.end() ; ++it)
	{
		cmdType type = getCommandType(*it);
		std::cout << "enum type: " << type << " ." << std::endl;
		switch (type)
		{
			case (CMD_CAP):
				break ;
			case (CMD_QUIT):
				server.handleDisconnection(user.getFd());
				return (1);
			case (CMD_PASS):
				cmdPass(user, *it, server.getPassword());
				break ;
			case (CMD_SETNICK):
				cmdNick(user, server, *it);
				break ; // always returns 0, nick can always be changed.
			case (CMD_JOIN):
				cmdJoin(user, server, *it);
				break ;
			case (CMD_SETUNAME):
				cmdUser(user, *it);
				break ;
			case (CMD_SEND):
				cmdPrivMsg(user, server, *it);
				break ;
			case (CMD_PART):
				cmdPart(user, server, *it);
				break ;
			case (SEND_MSG):
				break ;
		}
	}
	return (0);
}

cmdType Command::getCommandType(const std::string &cmd)
{
	std::string keyWord = cmd.substr(0, cmd.find(' '));

	if (keyWord == "CAP") return (CMD_CAP);
	else if (keyWord == "QUIT") return (CMD_QUIT);
	else if (keyWord == "PASS") return (CMD_PASS);
	else if (keyWord == "JOIN") return (CMD_JOIN);
	else if (keyWord == "NICK") return(CMD_SETNICK);
	else if (keyWord == "USER") return (CMD_SETUNAME);
	else if (keyWord == "PRIVMSG") return (CMD_SEND);
	else if (keyWord == "PART") return (CMD_PART);
	else return (SEND_MSG);      
}

int	Command::cmdUser(Client &user, std::string command)
{
	std::string response;
	std::vector<std::string> cmdSplittedSpace = strTool.stringSplit(command, ' ');

	if (command.empty())
	{
	// Construir el mensaje de vuelta
		response = message.getMessages(461, user);
		message.sendMessage(user, response);
		return (1);
	}
	user.setUsername(cmdSplittedSpace[1]);
	user.setHasUser(true);
	response = "Username set to " + cmdSplittedSpace[1] + "\r\n";
	message.sendMessage(user, response);
	return (0);
}


int	Command::cmdPass(Client &user, std::string command, std::string pass)
{
	std::string response;
	std::vector<std::string> cmdSplittedSpace = strTool.stringSplit(command, ' ');
	if (command.empty() || pass.empty())
	{
	// Construir el mensaje de vuelta
		response = message.getMessages(461, user);
		message.sendMessage(user, response);
		return (1);
	}

//		std::cout << getPassword().length() << pass.length() << std::endl;
	if (cmdSplittedSpace[1] == pass)
	{
		response = "Password accepted.\r\n";
		message.sendMessage(user, response);
		user.setHasPass(true);
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
int Command::cmdNick(Client &user, Server &server, std::string cmd)
{
	std::vector<std::string> cmdSplittedSpace = strTool.stringSplit(cmd, ' ');
	std::string newNick = cmdSplittedSpace[1];

	nickReturn validationResult = checkNick(newNick);
	strTool.printBuffer(strTool.stringToHex(newNick));
	switch (validationResult) 
	{
		case NICK_OK:
			if (!user.getLogin() && !server.isNicknameInUse(newNick)) 
			{
				server.registerNickname(newNick, &user);
				user.setNickname(newNick);
				user.setHasNick(true);
				message.sendMessage(user, "Nickname set to " + newNick + "\r\n");
				return NICK_OK;
			} 
			else if (server.isNicknameInUse(newNick)) {
				message.sendMessage(user, " Error. " + newNick + " is already in use.\r\n");
			} 
			else 
			{
				server.unregisterNickname(user.getNickname());
				server.registerNickname(newNick, &user);
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


int Command::cmdJoin(Client &user, Server &server, std::string cmd)
{
	std::vector<std::string> cmdSplittedSpace = strTool.stringSplit(cmd, ' ');
	std::string channelName = cmdSplittedSpace[1]; 
	std::string response;

	std::map<int, Channel *>::iterator it;


	if (channelName.empty())
	{
		response = message.getMessages(461, user);
		message.sendMessage(user, response);
		return (0);
	}

	it = server._channels.begin();

	while (it != server._channels.end())
	{
		if (it->second->getChannelName() == channelName)
		{
			it->second->addUser(user.getFd(), user);

			// Notificar a todos en el canal sobre el nuevo usuario
			response = ":" + user.getPrefix() + " JOIN " + channelName + "\r\n";
			it->second->broadcastMessage(response);

			// Enviar el tema del canal si tiene uno
			std::string topic = it->second->getTopic();
			if (!topic.empty())
			{
				response = ":irc.middleman.org 332 " + user.getNickname() + " " + channelName + " :" + topic + "\r\n";
				message.sendMessage(user, response);
			}
			message.sendChannelNames(*it->second, user);
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
	server._channels[user.getFd()] = newChannel;
	newChannel->addUser(user.getFd(), user);

	// Notificar a todos en el canal sobre el nuevo usuario
	response = ":" + user.getPrefix() + " JOIN " + channelName + "\r\n";
	newChannel->broadcastMessage(response);

	message.sendChannelNames(*newChannel, user);
	return (0);
}

int Command::cmdPrivMsg(Client &user, Server &server, std::string command) {
	std::string response;
	std::vector<std::string> cmdSplittedSpace = strTool.stringSplit(command, ' ');

	if (command.empty())
	{
		response = message.getMessages(461, user);
		message.sendMessage(user, response);
		return (0);
	}

	std::string target = cmdSplittedSpace[1];
	if (cmdSplittedSpace.size() > 3)
	{
		for (size_t i = 3; i < cmdSplittedSpace.size(); ++i)
		{
			cmdSplittedSpace[2] += " " + cmdSplittedSpace[i];
		}
	}
	std::string msg = cmdSplittedSpace[2];
	//remove the colon from the message
	msg = msg.substr(1, msg.size());
	if (target.empty() || msg.empty())
	{
		response = message.getMessages(461, user);
		message.sendMessage(user, response);
		return (0);
	}

	std::map<std::string, Client*>::iterator it = server._nicknameMap.find(target);
	if (it != server._nicknameMap.end())
	{
		response = ":" + user.getPrefix() + " PRIVMSG " + target + " :" + msg + "\r\n";
		message.sendMessage(*it->second, response);
	}
	else
	{
		response = "Error. " + target + " is not in the channel.\r\n";
		message.sendMessage(user, response);
	}
	return (0);
}

int Command::cmdPart(Client &user, Server &server, std::string command) {
	std::string response;
	std::vector<std::string> cmdSplittedSpace = strTool.stringSplit(command, ' ');

	if (command.empty())
	{
		response = message.getMessages(461, user);
		message.sendMessage(user, response);
		return (0);
	}

	std::string channelName = cmdSplittedSpace[1];
	if (channelName.empty())
	{
		response = message.getMessages(461, user);
		message.sendMessage(user, response);
		return (0);
	}

	std::map<int, Channel*>::iterator it = server._channels.begin();
	while (it != server._channels.end())
	{
		if (it->second->getChannelName() == channelName)
		{
			it->second->removeUser(user.getFd());
			response = ":" + user.getPrefix() + " PART " + channelName + "\r\n";
			it->second->broadcastMessage(response);
			return (0);
		}
		++it;
	}
	response = "Error. " + channelName + " is not in the channel.\r\n";
	message.sendMessage(user, response);
	return (0);
}
