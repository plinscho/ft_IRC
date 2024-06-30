#include "Server.hpp"
#include <string>
#include <vector>

std::vector<std::string> stringSplit(const char *str, const char& c)
{
    std::string buff = "";           // Buffer to build each substring
    std::vector<std::string> split;  // Vector to hold the resulting substrings

    for (int i = 0; str[i] != '\0'; ++i)
    {
        if (str[i] != c)
        {
            buff += str[i];  // Append character to buffer if not the delimiter
        }
        else
        {
            if (!buff.empty())
            {
                split.push_back(buff);  // Add buffer to vector if it's not empty
                buff.clear();           // Clear the buffer for the next substring
            }
        }
    }

    // If there is any remaining buffer, add it to the vector
    if (!buff.empty())
    {
        split.push_back(buff);
    }

    return split;
}

cmdType getCommandType(const std::string &cmd)
{
    if (cmd == "CAP") return (CMD_LOGIN);
    else if (cmd == "JOIN") return (CMD_JOIN);
    else if (cmd == "/setnick") return(CMD_SETNICK);
    else if (cmd == "/setuname") return (CMD_SETUNAME);
    else if (cmd == "/send") return (CMD_SEND);
    else if (cmd == "/help") return (CMD_HELP);
    else return (SEND_MSG);    
}

int	checkNick(std::string newNick)
{
	if (newNick.empty())
		return EMPTY_NICK;
	if (newNick.size() > 8)
		return SIZE_EXCEED;
	if (newNick.find(' ') != std::string::npos)
		return HAS_SPACE;
	for (ssize_t i = 0 ; newNick[i] != '\0'; ++i)
	{
		unsigned char c = newNick[i];
		if (!isalnum(c))
			return IS_NOT_ALNUM;
	}
	return NICK_OK;
}

int	setNick(int type, Client *user, std::string newNick)
{
	switch (type)
	{
		case NICK_OK:
		{
			user->setNickname(newNick);
			if (send(user->getFd(), "Nickname succesfully changed\n", 30, MSG_DONTWAIT) == -1)
				return (-1);
			break;
		}
		case EMPTY_NICK:
		{
			if (send(user->getFd(), "Error. Empty nick is not allowed\n", 34, MSG_DONTWAIT) == -1)
				return (-1);
			break;
		}
		case SIZE_EXCEED:
		{
			if (send(user->getFd(), "Error. Nick is more than 8 chars\n", 34, MSG_DONTWAIT) == -1)
				return (-1);
			break;
		}
		case HAS_SPACE:
		{
			if (send(user->getFd(), "Error. Space chars in nick are not allowed\n", 44, MSG_DONTWAIT) == -1)
				return (-1);
			break;
		}
		case IS_NOT_ALNUM:
		{
			if (send(user->getFd(), "Error. Non alnum chars in nick detected\n", 41, MSG_DONTWAIT) == -1)
				return (-1);
			break;
		}
	}
	return (0);
}



