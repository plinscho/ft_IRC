#include "Command.hpp"
#include "Server.hpp"
#include <cstdlib>
#include <iostream>
#include <sstream>

static int paramsCheck(Server &server, Client &user, std::string params) {
    if (params.empty()) {
        std::string response = server.message.getMessages(461, user);
        server.message.sendMessage(user, response);
        return (1);
    }
    return (0);
}

// partir esta funcion en varias
int Command::execute(Client &user, Server &server) {
    // Save the vector to iterate through all the commands (specially in the
    // login)
    std::vector<std::string> commands =
        strTool.stringSplit(user.getRecvBuffer(), "\r\n");

    //	strTool.printBuffer("TEST" + trimmedStr + "ENDOFTEST");
    for (size_t i = 0; i < commands.size(); ++i) {
        std::vector<std::string> splittedCmd =
            strTool.stringSplit(commands[i], ' ');
        cmdType type = getCommandType(splittedCmd[0]);
        std::cout << "enum type: " << type << " ." << std::endl;
        switch (type) {
        case (CMD_CAP):
            break;
        case (CMD_QUIT):
            server.handleDisconnection(user.getFd());
            return (1);
        case (CMD_PASS):
            if (splittedCmd.size() > 1 &&
                !paramsCheck(server, user, splittedCmd[1]))
                cmdPass(user, splittedCmd[1], server.getPassword());
            break;
        case (CMD_SETNICK):
            if (splittedCmd.size() > 1 &&
                !paramsCheck(server, user, splittedCmd[1]))
                cmdNick(user, server, splittedCmd[1]);
            break; // always returns 0, nick can always be changed.
        case (CMD_JOIN):
            cmdJoin(user, server, splittedCmd);
            break;
        case (CMD_SETUNAME):
            if (!paramsCheck(server, user, splittedCmd[1]))
                cmdUser(user, splittedCmd[1]);
            break;
        case (CMD_SEND):
            if (!paramsCheck(server, user, splittedCmd[1]))
                cmdPrivMsg(user, server, splittedCmd);
            break;
        case (CMD_PART):
            if (!paramsCheck(server, user, splittedCmd[1]))
                cmdPart(user, server, splittedCmd[1]);
            break;
        case (CMD_TOPIC):
            if (!paramsCheck(server, user, splittedCmd[1]))
                cmdTopic(user, server, splittedCmd);
            break;
        case (CMD_KICK):
            if (!paramsCheck(server, user, splittedCmd[1]) &&
                !paramsCheck(server, user, splittedCmd[2]))
                cmdKick(user, server, splittedCmd[1], splittedCmd[2]);
            break;
        case (CMD_MODE):
            if (!paramsCheck(server, user, splittedCmd[1]))
                cmdMode(user, server, splittedCmd);
            break;
        case (CMD_INVITE):
            if (!paramsCheck(server, user, splittedCmd[1]) &&
                !paramsCheck(server, user, splittedCmd[2]))
                cmdInvite(user, server, splittedCmd[2], splittedCmd[1]);
            break;
        default:
            break;
        }
    }
    return (0);
}

cmdType Command::getCommandType(const std::string &keyWord) {
    if (keyWord == "CAP")
        return (CMD_CAP);
    else if (keyWord == "QUIT")
        return (CMD_QUIT);
    else if (keyWord == "PASS")
        return (CMD_PASS);
    else if (keyWord == "JOIN")
        return (CMD_JOIN);
    else if (keyWord == "NICK")
        return (CMD_SETNICK);
    else if (keyWord == "USER")
        return (CMD_SETUNAME);
    else if (keyWord == "PRIVMSG")
        return (CMD_SEND);
    else if (keyWord == "PART")
        return (CMD_PART);
    else if (keyWord == "TOPIC")
        return (CMD_TOPIC);
    else if (keyWord == "KICK")
        return (CMD_KICK);
    else if (keyWord == "MODE")
        return (CMD_MODE);
    else if (keyWord == "INVITE")
        return (CMD_INVITE);
    else
        return (SEND_MSG);
}

int Command::cmdUser(Client &user, std::string newUser) {
    std::string response;

    user.setUsername(newUser);
    user.setHasUser(true);
    response = "Username set to " + newUser + "\r\n";
    message.sendMessage(user, response);
    return (0);
}

int Command::cmdPass(Client &user, std::string &userPass, std::string pass) {
    std::string response;

    //		std::cout << getPassword().length() << pass.length() <<
    //std::endl;
    if (userPass == pass) {
        response = "Password accepted.\r\n";
        message.sendMessage(user, response);
        user.setHasPass(true);
        return (0);
    }
    return (1);
}
