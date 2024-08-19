#include "../server/Server.hpp"
#include "../server/Command.hpp"
#include <algorithm>

static int updateServerNick(Server &server, std::string &oldNick, std::string &newNick)
{
	std::map<std::string, Client *>::iterator it;

	it = server._nicknameMap.find(oldNick);

	// If the oldNick exists in the map
    if (it != server._nicknameMap.end()) {
        // Get the associated Client* pointer
        Client *client = it->second;

        // Erase the oldNick from the map
        server._nicknameMap.erase(it);

        // Insert the newNick with the previously stored Client* pointer
        server._nicknameMap[newNick] = client;

        return 0;
    }
	return (1);
}

// iterate through the users vector channelsJoined to update 
void	Server::updateChannelNick(Server &server, Client* user, std::string oldNickname, std::string newNick)
{
	std::vector<std::string>::iterator it;
	for (it = user->channelsJoined.begin() ; it != user->channelsJoined.end() ; ++it) {
		Channel *currentChannel = server.getChannelByName(*it);
		

		if (currentChannel->isUserOp(oldNickname)) {
			currentChannel->removeOpUser(oldNickname);
			currentChannel->addOpUser(newNick);
		}
		
		std::string response = ":" + oldNickname + "!" + user->getAddress() + " NICK :" + newNick + "\r\n";
		currentChannel->broadcastMessage(response);
	}
	updateServerNick(server, oldNickname, newNick);
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
int Command::cmdNick(Client &user, Server &server, std::string newNick)
{
	std::string oldNick = user.getNickname();
	nickReturn validationResult = checkNick(newNick);

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
				std::string response = ":" + oldNick + "!" + user.getAddress() + " NICK :" + newNick + "\r\n";
				message.sendMessage(user, response); //"Nickname changed to " + newNick + "\r\n");
				server.updateChannelNick(server, &user, oldNick, newNick);
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
