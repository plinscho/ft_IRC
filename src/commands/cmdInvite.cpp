#include "../server/Server.hpp"
#include "../server/Command.hpp"
#include <iostream>

int Command::cmdInvite(Client &user, Server &server, std::string channelName, std::string target) {

	std::string response;
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