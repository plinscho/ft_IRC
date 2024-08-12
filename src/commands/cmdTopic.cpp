#include "../server/Server.hpp"
#include "../server/Command.hpp"
#include <iostream>

int Command::cmdTopic(Client &user, Server &server, std::vector<std::string> &cmdSplittedSpace) {

	std::string response;
	std::string channelName = cmdSplittedSpace[1];
	std::map<std::string, Channel*>::iterator it = server._channels.begin();
	
	while (it != server._channels.end())
	{
		if (it->second->getChannelName() == channelName)
		{
			if (cmdSplittedSpace.size() == 2)
			{
				if (it->second->getTopic().empty()) {
					response = ":" + server.getServerName() + " 331 " + user.getNickname() + " " + channelName + " :No topic is set.\r\n";
					message.sendMessage(user, response);
					return (0);
				} else {
					response = ":" + server.getServerName() + " 332 " + user.getNickname() + " " + channelName + " " + it->second->getTopic() + "\r\n";
					message.sendMessage(user, response);
					return (0);
				}
			}
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