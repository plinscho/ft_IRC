#include "Server.hpp"

/*
	OTHER FUNCTIONS THAT ARE NOT METHODS

// Most usually the flag is MSG_DONTWAIT
void	sendMsgFd(int destFd, std::string msg, int flag)
{

}

// Most usually the flag is MSG_DONTWAIT
int		recvMsgFd(int originFd, char *buffer, size_t maxLen, int flag)
{

}
*/

int	checkNick(std::string newNick)
{
	if (newNick.empty())
		return EMPTY_NICK;
	if (newNick.size() > 8)
		return SIZE_EXCEED;
	if (newNick.find(' '))
		return HAS_SPACE;
	for (int i = 0 ; newNick.size() ; i++)
	{
		char c = newNick[i];
		if (!isalnum(c))
			return IS_NOT_ALNUM;
	}
	return NICK_OK;
}

int	quickError(std::string msg, int errcode)
{
	std::cerr << msg << std::endl;
	return (errcode);
}

int sendWelcome(int fd)
{
	char buffer[] = "___________________________________________\
	\n\tMIDDLEMAN IRC SERVER\n___________________________________________\
	\nCOMMANDS:\
	\n/login\t\t<password> <nickname>\
	\n/join\t\t<channel>\
	\n/setnick\t<new nick>\t(max 8 characters)\
	\n/setuname\t<new username>\t(max 8 characters)\
	\n/send\t\t<ip addr> | <nickname>\t(send a private msg)\
	\n/help\t\t(print this message again)\n\
	\n";

	if (fd < 0 || send(fd, buffer, sizeof(buffer), MSG_DONTWAIT) == -1)
		return (quickError("Error.\nWecolme message could not be send!", EXIT_FAILURE));
	return (0);
}