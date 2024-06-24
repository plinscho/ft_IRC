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
	if (newNick.size() > 12)
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