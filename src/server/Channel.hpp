#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "../messages/Messages.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>


class Server;
class Client;

class Channel
{

	class ChannelMode {
	private:
			std::string 				_newMode;
			std::string					_modes;
			bool						_invite;			// i
			bool						_topicRestrict;		// t
			bool						_key;				// k
			bool						_limit;				// l

	public:
			void setMode(std::string mode) {
					_newMode = mode;
					_modes += mode;
				}
			std::string	getCurrentChannelMode() const { 
				return "+" + _modes;
			 };
	};

	private:

		int					 					_channelId;
		std::string								_channelPass;
		std::string 							_channelName;
		std::string								_topic;

	public:

		ChannelMode								_mode;
		std::map<int, Client*>					_fdUsersMap;
		std::vector<std::string>				nickOp;		// vector str de nicks que son operadores
		int										activeUsers;
		Messages								message;
		int										maxUsers;

		Channel(int id, const std::string channelName);
		~Channel();
		int 									setNewId();
		int										getChannelId(void);
		std::string 							getChannelName(void);
		std::vector<std::string>				getChannelsNicks();
		std::string								getTopic(void);
		void									setTopic(std::string &topic);
		void									addUser(int fd, Client &newUser);
		bool 									isUserOp(std::string nickInChannel);
		void									removeUser(int fd);
		void									removeOpUser(std::string userNick);
		void 									broadcastMessage(const std::string &message);
		void 									broadcastMessageExcludeSender(Client *who, const std::string &msg);

};



#endif
