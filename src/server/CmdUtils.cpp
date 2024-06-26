#include "Server.hpp"
#include "../client/Client.hpp"
#include <string>
#include <vector>

const std::vector<std::string> stringSplit(const std::string& str, const char& c)
{
	std::string buff = "";
	std::vector<std::string> split;
    std::string::const_iterator strIt;

        
    if (str.empty()) { return (split); }
	
	for(strIt = str.begin() ; strIt != str.end() ; ++strIt)
	{
		if(*strIt != c)
            buff+=*strIt; 
        else
        {
            if(buff != "")
            {   
                split.push_back(buff);
                buff = "";
            }
        }
	}
	if(buff != "")
        split.push_back(buff);
	return split;
}

cmdType getCommandType(const std::string &cmd)
{
    if (cmd == "/login") return (CMD_LOGIN);
    else if (cmd == "/join") return (CMD_JOIN);
    else if (cmd == "/setnick") return(CMD_SETNICK);
    else if (cmd == "/setuname") return (CMD_SETUNAME);
    else if (cmd == "/send") return (CMD_SEND);
    else if (cmd == "/help") return (CMD_HELP);
    else return (SEND_MSG);    
}

int Server::handleInput (char *buffer, Client *user)
{
    if (!buffer)
        return (1);
    
    std::vector<std::string> cmd;

    cmd = stringSplit(std::string(buffer), ' ');
    if (cmd.empty())
        return (0);
    cmdType type = getCommandType(cmd[0]);
	std::cout << "Command: " << cmd[0] << std::endl;
    switch (type)
    {
        case (CMD_LOGIN):
            return cmdLogin(cmd, user);
        case (CMD_JOIN):
            return (cmdJoin(cmd, user));
        case (CMD_SETNICK):
            return (cmdSetNick(cmd, user));
        case (CMD_SETUNAME):
            return (cmdSetUname(cmd, user));
        case (CMD_SEND):
            return (cmdSend(cmd, user));
        case (CMD_HELP):
            return (cmdHelp(cmd, user));
        case (SEND_MSG):
            return type;
    }
    return (0);
}

