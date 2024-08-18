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

typedef struct mode_s
{
	Server		*serverPtr;
	Channel 	*currentChannel;
	bool 		addMode;		 	// True if adding modes, false if removing
	size_t 		paramIndex;	// Index of parameters following the mode string
	std::string channelName;
	int			limitUsersInChannel;

}t_mode;

void	modeInvite(bool addMode, Channel &currentChannel)
{
	if (addMode && !currentChannel._mode.getInvite()) {
		currentChannel._mode.setMode("i");
	} else if (!addMode && currentChannel._mode.getInvite()) {
		currentChannel._mode.unsetMode("i");
	}
}

void	modeTopic(bool addMode, Channel &currentChannel)
{
	if (addMode && !currentChannel._mode.getTopic()) {
		currentChannel._mode.setMode("t");
	} else if (!addMode && currentChannel._mode.getTopic()){
		currentChannel._mode.unsetMode("t");
	}
}

int	modePassword(t_mode &mode, Client &user, std::vector<std::string> &cmdSplittedSpace)
{
	if (mode.paramIndex < cmdSplittedSpace.size()) return (461);

	std::cout << "BBBBBBBBBBB\n" << std::endl;
	if (mode.addMode && mode.currentChannel->_mode.getKey()){
		std::string password = cmdSplittedSpace[mode.paramIndex++];
		mode.currentChannel->_mode.setMode("k");
		mode.currentChannel->setChannelKey(password);
		std::string response = ":" + mode.serverPtr->getServerName() + " " + user.getNickname() + " " + mode.channelName + \
			" :" + user.getNickname() + " sets channel keyword to " + password + "\r\n";
		mode.currentChannel->broadcastMessage(response);
		return (0);
	}
	else if (!mode.addMode && mode.currentChannel->_mode.getKey())
	{
		if (cmdSplittedSpace[mode.paramIndex++] == mode.currentChannel->getChannelKey()){
			mode.currentChannel->_mode.unsetMode("k");
			mode.currentChannel->removeChannelKey();
		}
		mode.currentChannel->broadcastMessage("Password was removed\r\n");
		return (0);
	}
	else
	{
		return (411);
	}
	return (0);
}

int modeOperator(t_mode &mode, std::vector<std::string> &cmdSplittedSpace)
{
	if (mode.paramIndex > cmdSplittedSpace.size()) return (461);

	std::string userName = cmdSplittedSpace[mode.paramIndex++];
	Client *targetUser = mode.serverPtr->getClientByName(userName);
	if (!targetUser) return (401);  // No user found
	if (mode.addMode && !mode.currentChannel->isUserOp(userName)) {
		mode.currentChannel->addOpUser(userName);
	} else if (!mode.addMode && mode.currentChannel->isUserOp(userName)){
		mode.currentChannel->removeOpUser(userName);
	}
	return (0);
}

int modeLimit(t_mode &mode, std::vector<std::string> &cmdSplittedSpace)
{
	if (mode.paramIndex > cmdSplittedSpace.size()) return (461);

	if (mode.addMode && !mode.currentChannel->_mode.getLimit()) {
		int limit = std::atoi(cmdSplittedSpace[mode.paramIndex++].c_str());
		mode.currentChannel->_mode.setMode("l");
		mode.currentChannel->setUserLimit(limit);
		if (limit >= 0){
			mode.limitUsersInChannel = limit;
		}
	} else if (!mode.addMode && mode.currentChannel->_mode.getLimit()){
		mode.currentChannel->_mode.unsetMode("l");
		mode.currentChannel->removeUserLimit();
		mode.limitUsersInChannel = -1;
	}
	return (0);
}

// END OF MODE FUNCTIONS

int modeFirstParse(Client &user, Server &server, std::vector<std::string> &cmdSplittedSpace)
{
	std::string channelName = cmdSplittedSpace[1];
	Channel *currentChannel = server.getChannelByName(channelName);

	// check if user can change the modes (if is op)
	if (!currentChannel) {
		std::string response = server.message.getMessages(403, user, "", cmdSplittedSpace[1]); // 403: ERR_NOSUCHCHANNEL
		server.message.sendMessage(user, response);
		return (1);
	}
	else if (!currentChannel->isUserOp(user.getNickname())) 
	{
        std::string response = server.message.getMessages(482, user, "", currentChannel->getChannelName());
        server.message.sendMessage(user, response);
		return (1);
	}

	// Send error message for non-existing channel
	if (cmdSplittedSpace[1] == server.getServerName())
	{
		server.message.sendMessage(user, "You cannot change other user permisions.\r\n");
		return (1);
	}
	// Send the current modes of the channel
	if (cmdSplittedSpace.size() == 2) {
		std::string modes = currentChannel->_mode.getCurrentChannelMode();
		std::string response = server.message.getMessages(324, user); // 324: RPL_currentChannelEIS
		response += channelName + " " + modes + "\r\n";
		server.message.sendMessage(user, response);
		return (1);
	}
	return (0);
}

int		Command::cmdMode(Client &user, Server &server, std::vector<std::string> &cmdSplittedSpace)
{
	if (modeFirstParse(user, server, cmdSplittedSpace))
		return (1);//error, do something
	
	t_mode modeStruct;

	modeStruct.serverPtr = &server;
	modeStruct.channelName = cmdSplittedSpace[1];
	modeStruct.currentChannel = server.getChannelByName(modeStruct.channelName);
	modeStruct.addMode = true;
	modeStruct.paramIndex = 3;
	modeStruct.limitUsersInChannel = 0;

    // Check if currentChannel is valid
    if (!modeStruct.currentChannel) {
        std::string response = server.message.getMessages(403, user, "", modeStruct.channelName); // 403: ERR_NOSUCHCHANNEL
        server.message.sendMessage(user, response);
        return (1);
    }

	// Append modes to be changed
	std::string modes;
	for (size_t i = 2; i < cmdSplittedSpace.size(); ++i) {
		modes += cmdSplittedSpace[i];
	}
	int errorKey, errorLimit, errorOp;
	char key;
	for (size_t i = 0; i < modes.size(); ++i) {
		key = modes[i];
		switch (key) {
			case '+':
				modeStruct.addMode = true;
				break;
			case '-':
				modeStruct.addMode = false;
				break;
			case 'i':
				modeInvite(modeStruct.addMode, *modeStruct.currentChannel);
				break;
			case 't':
				modeTopic(modeStruct.addMode, *modeStruct.currentChannel);
				break;
			case 'k':
				errorKey = modePassword(modeStruct, user, cmdSplittedSpace);
				if (errorKey){
					std::string response = server.message.getMessages(errorKey, user);
					server.message.sendMessage(user, response);
				}
				break;
			case 'o':
				errorOp = modeOperator(modeStruct, cmdSplittedSpace);
				if (errorOp){
					std::string response = server.message.getMessages(errorOp, user);
					server.message.sendMessage(user, response);
				}
				break;
			// hasta aqui
			case 'l':
				errorLimit = modeLimit(modeStruct, cmdSplittedSpace);
				if (errorLimit)
				{
					std::string response = server.message.getMessages(errorLimit, user);
					server.message.sendMessage(user, response);	
				}
				if (modeStruct.limitUsersInChannel > 0 ){		
					std::ostringstream ss;
					ss << modeStruct.limitUsersInChannel;
					std::string strLimit = ss.str();
					std::string response = ":" + server.getServerName() + " " + user.getNickname() + " " + modeStruct.channelName + \
					" :" + user.getNickname() + " sets channel limit to " + strLimit + "\r\n";
					message.sendMessage(user, response);
				}
				else if (modeStruct.limitUsersInChannel < 0 ) // removed limit
				{
					std::string response = ":" + server.getServerName() + " " + user.getNickname() + " " + modeStruct.channelName + \
					" :" + user.getNickname() + " removed user limit" + "\r\n";
					message.sendMessage(user, response);
				}
				break;
			default:
				// Send error message for unknown mode character
				std::string response = message.getMessages(472, user); // 472: ERR_UNKNOWNMODE
				response += key;
				message.sendMessage(user, response);
				break;
		}
	}
	// Send the updated mode information to the user
	std::string updatedModes = modeStruct.currentChannel->_mode.getCurrentChannelMode();
	std::string response = message.getMessages(324, user); // 324: RPL_currentChannelEIS
	response += modeStruct.channelName + " " + updatedModes + "\r\n";
	message.sendMessage(user, response);
	return (0);
}
