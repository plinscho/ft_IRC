#ifndef CHANNEL_HPP
#define CHANNEL_HPP

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
		std::string								_topic;

	public:
		std::map<int, Client*>					_fdUsersMap;
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
		void									removeUser(int fd);
		void 									broadcastMessage(const std::string &message);
		void 									broadcastMessageExcludeSender(Client *who, const std::string &msg);

};


#endif
