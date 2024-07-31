#include "Command.hpp"
#include "Server.hpp"
#include <iostream>
#include <cstdlib>

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
			case (CMD_TOPIC):
				cmdTopic(user, server, *it);
				break ;
			case (CMD_KICK):
				cmdKick(user, server, *it);
				break ;
			case (CMD_MODE):
				cmdMode(user, server, *it);
				break ;
			case (CMD_INVITE):
				cmdInvite(user, server, *it);
				break ;
			default:
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
	else if (keyWord == "TOPIC") return (CMD_TOPIC);
	else if (keyWord == "KICK") return (CMD_KICK);
	else if (keyWord == "MODE") return (CMD_MODE);
	else if (keyWord == "INVITE") return (CMD_INVITE);
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
	if (target[0] == '#')
	{

		std::map<std::string, Channel*>::iterator it = server._channels.begin();
		//search #channel
		while (it != server._channels.end())
		{
			if (it->second->getChannelName() == target)
			{
				response = ":" + user.getPrefix() + " PRIVMSG " + target + " :" + msg + "\r\n";
				it->second->broadcastMessageExcludeSender(&user, response);
				return (0);
			}
			++it;
		}	
	} else
	{ 
		std::map<std::string, Client*>::iterator it = server._nicknameMap.find(target);
		if (it != server._nicknameMap.end())
		{
			response = ":" + user.getPrefix() + " PRIVMSG " + target + " :" + msg + "\r\n";
			message.sendMessage(*it->second, response);
		}
		else
		{ //TODO: check also +nv  404     ERR_CANNOTSENDTOCHAN 
			response = "Error. " + target + " is not in the channel.\r\n";
			message.sendMessage(user, response);
		}
	
	}
	return (0);
}

// Modificar topic checkear si el user es op
int Command::cmdTopic(Client &user, Server &server, std::string command) {
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
	if (cmdSplittedSpace.size() == 2)
	{
		response = channelName + " :No topic is set.\r\n";
		message.sendMessage(user, response);
		return (0);
	}
	
	std::map<std::string, Channel*>::iterator it = server._channels.begin();
	
	while (it != server._channels.end())
	{
		if (it->second->getChannelName() == channelName)
		{
			if (it->second->_mode.getTopic() && !it->second->isUserOp(user.getNickname()))
			{
				// user cannot change topic
				response = message.getMessages(482, user, "", channelName);
				message.sendMessage(user, response);
				return (0);
			}
			if (cmdSplittedSpace.size() > 3)
			{
				for (size_t i = 3; i < cmdSplittedSpace.size(); ++i)
				{
					cmdSplittedSpace[2] += " " + cmdSplittedSpace[i];
				}
			}
			std::string topic = cmdSplittedSpace[2];
			//remove the colon from the message
			topic = topic.substr(1, topic.size());
			it->second->setTopic(topic);
			response = ":" + user.getPrefix() + " TOPIC " + channelName + " :" + topic + "\r\n";
			it->second->broadcastMessage(response);
			return (0);
		}
		++it;
	}
	response = "Error. " + channelName + " is not in the channel.\r\n";
	message.sendMessage(user, response);
	return (0);
}


// COMANDO hecho por HexChat: KICK <nombreCanal> <user>
int Command::cmdKick(Client &user, Server &server, std::string command) {
	std::string response;
	std::vector<std::string> cmdSplittedSpace = strTool.stringSplit(command, ' ');

	if (command.empty() || cmdSplittedSpace.size() != 3) {
		response = message.getMessages(461, user);
		message.sendMessage(user, response);
		return (0);
	}

	std::string channelName = cmdSplittedSpace[1];
	std::string target = cmdSplittedSpace[2];

	if (channelName.empty() || target.empty()) {
		response = message.getMessages(461, user);
		message.sendMessage(user, response);
		return (0);
	}

	Channel *currentChnl = server.getChannelByName(channelName);
	if (!currentChnl) {
		response = "Error. " + channelName + " does not exist.\r\n";
		message.sendMessage(user, response);
		return (0);
	}

	if (currentChnl->getChannelName() == channelName) {
		std::map<int, Client*>::iterator it2 = currentChnl->_fdUsersMap.begin();
		while (it2 != currentChnl->_fdUsersMap.end()) {
			if (it2->second->getNickname() == target && currentChnl->isUserOp(user.getNickname())) {
				response = ":" + user.getPrefix() + " KICK " + channelName + " " + target + "\r\n";
				currentChnl->broadcastMessage(response);
				int fd = it2->first;
				std::string nickname = it2->second->getNickname();
				
				// Ensure no invalid access before removing
				if (currentChnl->_fdUsersMap.find(fd) != currentChnl->_fdUsersMap.end()) {
					currentChnl->removeUser(fd);
				}
				
				if (std::find(currentChnl->nickOp.begin(), currentChnl->nickOp.end(), nickname) != currentChnl->nickOp.end()) {
					currentChnl->removeOpUser(nickname);
				}

				return (0);
			// user is not operator and cannot /kick anyone
			} else if (it2->second->getNickname() == target) {
				response = "Error. " + user.getNickname() + " is not an operator.\r\n";
				message.sendMessage(user, response);
				return (0);
			}
			++it2;
		}
	}
	response = "Error. " + target + " is not in the channel.\r\n";
	message.sendMessage(user, response);
	return (0);
}


int Command::cmdInvite(Client &user, Server &server, std::string command) {
	std::string response;
	std::vector<std::string> cmdSplittedSpace = strTool.stringSplit(command, ' ');

	if (command.empty() || cmdSplittedSpace.size() != 3) {
		response = message.getMessages(461, user);
		message.sendMessage(user, response);
		return (0);
	}

	std::string channelName = cmdSplittedSpace[2];
	std::string target = cmdSplittedSpace[1];

	if (channelName.empty() || target.empty()) {
		response = message.getMessages(461, user);
		message.sendMessage(user, response);
		return (0);
	}

	Channel *currentChnl = server.getChannelByName(channelName);
	if (!currentChnl) {
		response = "Error. " + channelName + " does not exist.\r\n";
		message.sendMessage(user, response);
		return (0);
	}

	std::map<int, Client*>::iterator it2 = currentChnl->_fdUsersMap.begin();
	if (currentChnl->isUserOp(user.getNickname())) {
		response = ":" + user.getPrefix() + " INVITE " + target + "\r\n";
		message.sendMessage(*it2->second, response);
		currentChnl->addInvited(target);
		return (0);
	} else {
		response = "Error. " + user.getNickname() + " is not an operator.\r\n";
		message.sendMessage(user, response);
		return (0);
	}
	return (0);
}

/*
	Controla que usuarios del canal son OP
	Modifica el MODE del canal, puede ser
	i: Set/remove Invite-only channel
	t: Set/remove the restrictions of the TOPIC command to channel
		operators
	k: Set/remove the channel key (password)
	o: Give/take channel operator privilege
	l: Set/remove the user limit to channel

	MODE tiene acceso a la informacion de los canales y sus permisos
	Ejemplo de un comando MODE:

	MODE #canal +i -t +pk -pk
*/ 
int		Command::cmdMode(Client &user, Server &server, std::string command)
{
	// Split the command by space
	std::vector<std::string> cmdSplittedSpace = strTool.stringSplit(command, ' ');
	if (cmdSplittedSpace.size() < 2) {
		// Send error message for insufficient parameters
		std::string response = message.getMessages(461, user); // 461: ERR_NEEDMOREPARAMS
		message.sendMessage(user, response);
		return 0;
	}

	std::string channelName = cmdSplittedSpace[1];

	Channel *channelMod = server.getChannelByName(channelName);

	// Send error message for non-existing channel
	if (cmdSplittedSpace[1] == server.getServerName())
	{
		message.sendMessage(user, "You cannot change other user permisions.\r\n");
		return (0);
	}
	else if (!channelMod) {
		std::string response = message.getMessages(403, user, "", cmdSplittedSpace[1]); // 403: ERR_NOSUCHCHANNEL
		message.sendMessage(user, response);
		return 0;
	}
	// Send the current modes of the channel
	if (cmdSplittedSpace.size() == 2) {
		std::string modes = channelMod->_mode.getCurrentChannelMode();
		std::string response = message.getMessages(324, user); // 324: RPL_CHANNELMODEIS
		response += channelName + " " + modes + "\r\n";
		message.sendMessage(user, response);
		return 0;
	}


	// Si append modes to be changed
	std::string modes;
	for (size_t i = 2; i < cmdSplittedSpace.size(); ++i) {
		modes += cmdSplittedSpace[i];
	}

	bool addMode = true; // True if adding modes, false if removing
	size_t paramIndex = 3; // Index of parameters following the mode string

	for (size_t i = 0; i < modes.size(); ++i) {
		char key = modes[i];
		switch (key) {
			case '+':
				addMode = true;
				break;
			case '-':
				addMode = false;
				break;
			case 'i':
				if (addMode) {
					channelMod->_mode.setMode("i");
				} else {
					channelMod->_mode.unsetMode("i");
				}
				break;
			case 't':
				if (addMode) {
					channelMod->_mode.setMode("t");
				} else {
					channelMod->_mode.unsetMode("t");
				}
				break;
			case 'k':
				if (addMode) {
					if (paramIndex < cmdSplittedSpace.size()) {
						std::string key = cmdSplittedSpace[paramIndex++];
						channelMod->_mode.setMode("k");
						channelMod->setChannelKey(key);
					} else {
						// Send error message for missing parameter
						std::string response = message.getMessages(461, user); // 461: ERR_NEEDMOREPARAMS
						message.sendMessage(user, response);
						return 0;
					}
				} else {
					channelMod->_mode.unsetMode("k");
					channelMod->removeChannelKey();
				}
				break;
			case 'o':
				if (paramIndex < cmdSplittedSpace.size()) {
					std::string userName = cmdSplittedSpace[paramIndex++];
					Client *targetUser = server.getClientByName(userName);
					if (targetUser) {
						if (addMode) {
							channelMod->addOpUser(userName);
						} else {
							channelMod->removeOpUser(userName);
						}
					} else {
						// Send error message for non-existing user
						std::string response = message.getMessages(401, user); // 401: ERR_NOSUCHNICK
						response += userName + "\r\n";
						message.sendMessage(user, response);
						return 0;
					}
				} else {
					// Send error message for missing parameter
					std::string response = message.getMessages(461, user); // 461: ERR_NEEDMOREPARAMS
					message.sendMessage(user, response);
					return 0;
				}
				break;
			case 'l':
				if (addMode) {
					if (paramIndex < cmdSplittedSpace.size()) {
						int limit = std::atoi(cmdSplittedSpace[paramIndex++].c_str());
						channelMod->_mode.setMode("l");
						channelMod->setUserLimit(limit);
					} else {
						// Send error message for missing parameter
						std::string response = message.getMessages(461, user); // 461: ERR_NEEDMOREPARAMS
						message.sendMessage(user, response);
						return 0;
					}
				} else {
					channelMod->_mode.unsetMode("l");
					channelMod->removeUserLimit();
				}
				break;
			default:
				// Send error message for unknown mode character
				std::string response = message.getMessages(472, user); // 472: ERR_UNKNOWNMODE
				response += key;
				message.sendMessage(user, response);
				return 0;
		}
	}
	// Send the updated mode information to the user
	std::string updatedModes = channelMod->_mode.getCurrentChannelMode();
	std::string response = message.getMessages(324, user); // 324: RPL_CHANNELMODEIS
	response += channelName + " " + updatedModes + "\r\n";
	message.sendMessage(user, response);
	return (0);
}

/*
static int modeParser(std::string command)
{
	(void)command;
	return (1);
}
*/
