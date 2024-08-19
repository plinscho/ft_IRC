#include "../server/Server.hpp"
#include "../server/Command.hpp"

// iterate through the users vector channelsJoined to update 
void	Server::updateChannelNames(Server &server, Client* user, std::string newNick)
{
	std::vector<std::string>::iterator it;
	for (it = user->channelsJoined.begin() ; it != user->channelsJoined.end() ; ++it) {
		Channel *currentChannel = server.getChannelByName(*it);
		currentChannel->broadcastMessage("Nickname changed to " + newNick + "\r\n");
		server.message.sendChannelNames(*currentChannel, *user);
	}
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
				message.sendMessage(user, "Nickname changed to " + newNick + "\r\n");
				server.updateChannelNames(server, &user, newNick);
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