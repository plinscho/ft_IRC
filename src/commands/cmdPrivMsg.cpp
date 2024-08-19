#include "../server/Server.hpp"
#include "../server/Command.hpp"
#include <iostream>

int Command::cmdPrivMsg(Client &user, Server &server, std::vector<std::string> &cmdSplittedSpace) {

	std::string response;
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
