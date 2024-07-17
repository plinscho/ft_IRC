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

/*
	std::cout << "\nstringToHex(*it)" << std::endl;
	for (std::vector<std::string>::iterator it = split.begin() ; it != split.end() ; ++it)
	{
		std::cout << *it << std::endl;
	}
*/

    return split;
}

std::vector<std::string> stringNoTrimSplit(std::string str, std::string delimiter)
{
    std::vector<std::string> split;
    std::string substr;
    size_t start = 0, end = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos) {
        // Include the delimiter in the substring
        substr = str.substr(start, end - start + delimiter.size());
        split.push_back(substr);
        start = end + delimiter.size();
    }
    // Add the remaining part of the string after the last delimiter, if any
    substr = str.substr(start);
    if (!substr.empty()) {
        split.push_back(substr);
    }
    return split;
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

cmdType getCommandType(const std::string &cmd)
{
    if (cmd == "CAP") return (CMD_CAP);
	else if (cmd == "QUIT") return (CMD_QUIT);
	else if (cmd == "PASS") return (CMD_PASS);
    else if (cmd == "JOIN") return (CMD_JOIN);
    else if (cmd == "NICK") return(CMD_SETNICK);
    else if (cmd == "USER") return (CMD_SETUNAME);
    else if (cmd == "SEND") return (CMD_SEND);
    else return (SEND_MSG);      
}

bool	getLogStat(Client *user)
{
	if (user->getHasNick() && user->getHasPas() && user->getHasUser() && user->getLogin() == false)
		return (true);
	else
		return (false);
}





