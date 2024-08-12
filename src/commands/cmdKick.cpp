#include "../server/Command.hpp"
#include "../server/Server.hpp"
#include <iostream>


// COMANDO hecho por HexChat: KICK <nombreCanal> <user>
int Command::cmdKick(Client &user, Server &server, std::string channelName, std::string target) {

	std::string response;
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
