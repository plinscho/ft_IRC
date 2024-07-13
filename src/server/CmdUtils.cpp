#include "Server.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>

std::string stringToHex(const std::string& str) {
    std::ostringstream oss;

    // Iterar sobre cada carácter de la cadena
    for (std::size_t i = 0; i < str.size(); ++i) {
        // Obtener el valor numérico del carácter
        unsigned char c = static_cast<unsigned char>(str[i]);
        
        // Convertir a hexadecimal y agregar al stream
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
        
        // Añadir un espacio entre los caracteres para mayor legibilidad
        if (i != str.size() - 1) {
            oss << ' ';
        }
    }

    // Devolver la cadena hexadecimal resultante
    return oss.str();
}

std::vector<std::string> stringSplit(std::string str, std::string delimiter)
{
	std::vector<std::string> split;
	std::string substr;
	size_t start, end;

	start = 0;
	while ((end = str.find(delimiter, start)) != std::string::npos)
	{
		substr = str.substr(start, end - start);
		if (!substr.empty())
			split.push_back(substr);
		start = end + delimiter.size();
	}
	substr = str.substr(start);
	if (!substr.empty())
		split.push_back(substr);
	else
		split.push_back(delimiter);
	return (split);
}

std::vector<std::string> stringSplit(std::string str, char c)
{
	std::string buff;           // Buffer to build each substring
	std::vector<std::string> split;  // Vector to hold the resulting substrings
	std::istringstream ss(str);

	while (std::getline(ss, buff, c)){
		split.push_back(buff);
	}
	return split;
}

bool	toggleBool(bool state)
{
	if (state == true)
		return (false);
	else
		return (true);
}

// check that in handshake Vector exists a NICK, PASS, USER and CAP
bool		checkHandshakeOptions(Client *user)
{
	std::vector<std::string>::iterator it;
	bool hasCAP, hasNICK, hasPASS, hasUSER = false;

	for (it = user->handshakeVector.begin() ; it != user->handshakeVector.end() ; ++it)
	{
		if (it->find("CAP") != std::string::npos)
			hasCAP = toggleBool(hasCAP);
		if (it->find("PASS") != std::string::npos)
			hasPASS = toggleBool(hasPASS);
		if (it->find("NICK") != std::string::npos)
			hasNICK = toggleBool(hasNICK);
		if (it->find("USER") != std::string::npos)
			hasUSER = toggleBool(hasUSER);
	}
	return(hasCAP && hasNICK && hasUSER && hasPASS);
}

void	fillHandshakeVector(Client *user, std::string &clientHandshakeBuffer)
{
	size_t	pos = 0;

	while ((pos = clientHandshakeBuffer.find("\r\n")) != std::string::npos) // Limita las iteraciones para evitar bucle infinito
	{
		std::string substr = clientHandshakeBuffer.substr(0, pos); // Obtiene el substring hasta "\r\n"
		//substr.push_back('\0'); // Añade el carácter nulo al final
		user->handshakeVector.push_back(substr); // Realiza el push_back del resultado
		clientHandshakeBuffer.erase(0, pos + 2); // Elimina la parte procesada del buffer, incluyendo "\r\n"
	}

}

std::string trim(const std::string& str) 
{

    size_t first = str.find_first_not_of('\r');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of('\r');
    return str.substr(first, (last - first + 1));
}

cmdType getCommandType(const std::string &cmd)
{
    if (cmd == "CAP") return (CMD_CAP);
	else if (cmd == "PASS") return (CMD_PASS);
    else if (cmd == "JOIN") return (CMD_JOIN);
    else if (cmd == "NICK") return(CMD_SETNICK);
    else if (cmd == "USER") return (CMD_SETUNAME);
    else if (cmd == "/send") return (CMD_SEND);
    else if (cmd == "/help") return (CMD_HELP);
    else return (SEND_MSG);      
}

int	preCmdCheck(std::vector<std::string> cmd, Client *user)
{
	if (cmd.empty() || !user)
		return (-1);
	else if ((user->getLogin()) == false)
	{
		if (sendMessage(user, "Error: You need to be logged in before!\n") == -1)
			return (-1);
		else
			return (1);
	}
	return (0);
}

int	checkNick(std::string newNick, int fd)
{
	(void)fd;
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



