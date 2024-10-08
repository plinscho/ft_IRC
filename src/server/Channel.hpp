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

		bool getInvite(void)
		{
			return _invite;
		}
		
		bool getTopic(void)
		{
			return _topicRestrict;
		}

		bool getKey(void)
		{
			return _key;
		}

		bool getLimit(void)
		{
			return _limit;
		}

		void modeInit(void)
		{
			_newMode = "";
			_modes = "";
			_invite = false;			// i
			_topicRestrict = true;		// t
			_key = false;				// k
			_limit = false;				// l
		}

		void setMode(std::string mode) 
		{
			_newMode = mode;
			_modes += mode;
			if (mode == "k") _key = true;
			if (mode == "i") _invite = true;
			if (mode == "t") _topicRestrict = true; 
			if (mode == "l") _limit = true;
		}

		void unsetMode(std::string mode) 
		{
			size_t pos = _modes.find(mode);
			if (pos != std::string::npos) 
			{
				_modes.erase(pos, 1);
				if (mode == "k") _key = false;
				if (mode == "i") _invite = false;
				if (mode == "t") _topicRestrict = false;
				if (mode == "l") _limit = false;
			}
		}

		std::string	getCurrentChannelMode() const 
		{ 
			return "+" + _modes;
		};
	};

	private:
		std::string 							_channelName;
		std::string								_topic;
		std::string								_key;

	public:

		ChannelMode								_mode;
		std::map<int, Client*>					_fdUsersMap;
		std::vector<std::string>				nickOp;		// vector str de nicks que son operadores
		std::vector<std::string>				invitedUsers;
		int										activeUsers;
		Messages								message;
		int										maxUsers;
		bool									hasKey;
		Channel(const std::string channelName);
		~Channel();
		int 									setNewId();
		std::string 							getChannelName(void);
		std::vector<std::string>				getChannelsNicks();
		std::string								getTopic(void);
		std::string								getChannelKey(void); 
		void									setChannelKey(std::string &);
		void    								removeChannelKey(void);
		bool 									isInvited(std::string nick);
		void 									addInvited(std::string nick);
		void									setTopic(std::string &topic);
		void									addUser(int fd, Client &newUser);
		bool 									isUserOp(std::string nickInChannel);
		void									setUserLimit(int );
		void									removeUserLimit(void);
		void									removeUser(int fd);
		void    								addOpUser(std::string userNick);
		void									removeOpUser(std::string userNick);
		void 									broadcastMessage(const std::string &message);
		void 									broadcastMessageExcludeSender(Client *who, const std::string &msg);
};



#endif
