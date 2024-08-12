#include "../server/Server.hpp"
#include "../server/Command.hpp"
#include <iostream>

static int handlePart(Client &user, Server &server, const std::string channelName)
{
	std::string response;
//	quickError("CHannel name part: " + channelName, 1);
	Channel *currentChnl = server.getChannelByName(channelName);
//	std::cout << currentChnl->getChannelName() << std::endl;
	if (!currentChnl) 
	{
		response = "Error. " + channelName + " does not exist.\r\n";
		server.message.sendMessage(user, response);
		return (0);
	}
	int flag = 0;
	if (currentChnl->getChannelName() == channelName) 
	{
		std::map<int, Client*>::iterator it2 = currentChnl->_fdUsersMap.begin();
		while (!flag && it2 != currentChnl->_fdUsersMap.end()) 
		{
			int fd = it2->first;
			std::string nickname = it2->second->getNickname();
			// respuesta para el servidor (channel)
			response = ":" + user.getPrefix() + " PART " + channelName + "\r\n";
			currentChnl->broadcastMessage(response);
			
			// Ensure no invalid access before removing
			if (std::find(currentChnl->nickOp.begin(), currentChnl->nickOp.end(), nickname) != currentChnl->nickOp.end()) {
				currentChnl->removeOpUser(nickname);
			}
			if (currentChnl->_fdUsersMap.find(fd) != currentChnl->_fdUsersMap.end()) {
				currentChnl->removeUser(fd);
				std::cout << "parted user with fd: " + fd << std::endl;
				flag = 1;
			}
			if (!flag)
				++it2;
		}
		if (currentChnl->activeUsers == 0)
			server.deleteChannel(channelName);
		return (0);
	}
	return (1);
}

int Command::cmdPart(Client &user, Server &server, std::string channelNames)
{
	std::string response;

	std::vector<std::string> channelsPart = strTool.stringSplit(channelNames, ',');
	// Iterate over each channel name
	for (size_t i = 0; i < channelsPart.size(); ++i) {
		std::string channelName = channelsPart[i];
			   // Handle joining the channel
		if (handlePart(user, server, channelName)) {
			std::string response = "Error. No such channel: " + channelName + "\r\n";
			message.sendMessage(user, response);
		}
	}
	return (0);
}