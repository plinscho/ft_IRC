#ifndef COMAND_HPP
#define COMAND_HPP

#include "StringHandler.hpp"
#include "../messages/Messages.hpp"
#include <string>
#include <vector>

class Client;
class Server;

enum cmdType
{
	CMD_CAP = 0,
	CMD_QUIT,
	CMD_PASS,
	CMD_JOIN,
	CMD_SETNICK,
	CMD_SETUNAME,
	CMD_SEND,
    CMD_PART,
    CMD_TOPIC,
    CMD_KICK,
    CMD_MODE,
	SEND_MSG
};

struct Command 
{
    std::string                 stringCommands;
    StringHandler               strTool;
    Messages                    message;
	std::vector<std::string>    m_params;

    void                        getFromClientBuffer(const Client &user);
    cmdType                     getCommandType(const std::string &cmd);

    int                         execute(Client &user, Server &server);
    int	                        cmdUser(Client &user, std::string command);
    int	                        cmdPass(Client &user, std::string command, std::string serverPassword);
    int                         cmdNick(Client &user, Server &server, std::string command);
    int                         cmdJoin(Client &user, Server &server, std::string command);
    int                         cmdPrivMsg(Client &user, Server &server, std::string command);
    int                         cmdPart(Client &user, Server &server, std::string command);
    int                         cmdTopic(Client &user, Server &server, std::string command);
    int                         cmdKick(Client &user, Server &server, std::string command);
    int                         cmdMode(Client &user, Server &server, std::string command);

};


#endif // COMMAND_HPP