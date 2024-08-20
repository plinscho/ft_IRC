#ifndef COMAND_HPP
#define COMAND_HPP

#include "../messages/Messages.hpp"
#include "StringHandler.hpp"
#include <string>
#include <vector>

class Client;
class Server;

enum cmdType {
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
    CMD_INVITE,
    SEND_MSG
};

struct Command {
    StringHandler strTool;
    Messages message;

    cmdType getCommandType(const std::string &keyWord);

    int execute(Client &user, Server &server);
    int cmdUser(Client &user, std::string command);
    int cmdPass(Client &user, std::string &userPass,
                std::string serverPassword);
    int cmdNick(Client &user, Server &server, std::string newNick);
    int cmdQuit(Server &server, Client &user, std::string reason);
    int cmdJoin(Client &user, Server &server,
                std::vector<std::string> &cmdSplittedSpace);
    int cmdPrivMsg(Client &user, Server &server,
                   std::vector<std::string> &cmdSplittedSpace);
    int cmdPart(Client &user, Server &server, std::string command);
    int cmdTopic(Client &user, Server &server,
                 std::vector<std::string> &cmdSplittedSpace);
    int cmdKick(Client &user, Server &server, std::string channelName,
                std::string target);
    int cmdMode(Client &user, Server &server,
                std::vector<std::string> &cmdSplittedSpace);
    int cmdInvite(Client &user, Server &server, std::string channelName,
                  std::string target);
};

#endif // COMMAND_HPP
