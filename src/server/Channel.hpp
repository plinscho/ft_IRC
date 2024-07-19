#ifndef CHANNEL_H
#define CHANNEL_H

#include "../messages/Messages.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>


class Server;
class Client;

#define MAX_CHANNEL_USERS 10

class Channel
{

	private:
		int					 					_channelId;
		std::string 							_channelName;
		std::map<int, Client*>					_fdUsersMap;
		std::string								_topic;

	public:
		int										activeUsers;
		Messages								message;
		Channel();

		Channel(int id, const std::string channelName);
		~Channel();
		int 									setNewId();
		int										getChannelId(void);
		std::string 							getChannelName(void);
		std::vector<std::string>				getChannelsNicks();
		std::string								getTopic(void);
		void									setTopic(std::string &topic);
		void									addUser(int fd, Client &newUser);
		void 									broadcastMessage(const std::string &message);

};


#endif
