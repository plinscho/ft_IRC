#ifndef CHANNEL_H
#define CHANNEL_H

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

	public:
		int										activeUsers;
		Channel();
		Channel(int id, const std::string channelName);
		~Channel();
		int				getChannelId(void);
		std::string 	getChannelName(void);
		void			addUser(int fd, Client *newUser);

};

#endif
