#include "../server/Server.hpp"
#include "../server/Command.hpp"
#include <iostream>

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
int		Command::cmdMode(Client &user, Server &server, std::vector<std::string> &cmdSplittedSpace)
{
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

	// check if user can change the modes (if is op)
	if (!channelMod->isUserOp(user.getNickname())) return (1);

	// Append modes to be changed
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
				if (addMode && !channelMod->_mode.getInvite()) {
					channelMod->_mode.setMode("i");
				} else if (!addMode && channelMod->_mode.getInvite()) {
					channelMod->_mode.unsetMode("i");
				}
				break;
			case 't':
				if (addMode && !channelMod->_mode.getTopic()) {
					channelMod->_mode.setMode("t");
				} else if (!addMode && channelMod->_mode.getTopic()){
					channelMod->_mode.unsetMode("t");
				}
				break;
			case 'k':
				if (addMode && !channelMod->_mode.getKey()) {
					if (paramIndex < cmdSplittedSpace.size()) {
						std::string password = cmdSplittedSpace[paramIndex++];
						channelMod->_mode.setMode("k");
						channelMod->setChannelKey(password);
					} else if (!addMode && channelMod->_mode.getKey()){
						if (cmdSplittedSpace[paramIndex++] == channelMod->getChannelKey())
						channelMod->_mode.unsetMode("k");
						channelMod->removeChannelKey();
					}
				} else {
					std::string response = message.getMessages(461, user); // 461: ERR_NEEDMOREPARAMS
					message.sendMessage(user, response);
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
						if (limit >= 0) {
							std::ostringstream ss;
							ss << limit;
							std::string strLimit = ss.str();
							std::string response = ":" + server.getServerName() + " " + user.getNickname() + " " + channelName + " :" + user.getNickname() + " sets channel limit to " + strLimit + "\r\n";
							message.sendMessage(user, response);
						}
						return 0;
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
