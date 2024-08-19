#include "../server/Command.hpp"
#include "../server/Server.hpp"
#include <climits>
#include <cstddef>
#include <iostream>
#include <vector>

/*
        Controla que usuarios del canal son OP
        Modifica el MODE del canal, puede ser
        i: Set/remove Invite-only channel
        t: Set/remove the restrictions of the TOPIC command to channel
                operators
        k: Set/remove the channel key (password)
        o: Give/take channel operator privilege
        l: Set/remove the user limit to channel

        MODE tiene acceso a la informacion de los canales y sus permisos
        Ejemplo de un comando MODE:

        MODE #canal +i -t +pk -pk

*/

void modeInvite(bool addMode, Channel &currentChannel) {
    if (addMode && !currentChannel._mode.getInvite()) {
        currentChannel._mode.setMode("i");
    } else if (!addMode && currentChannel._mode.getInvite()) {
        currentChannel._mode.unsetMode("i");
    }
}

void modeTopic(bool addMode, Channel &currentChannel) {
    if (addMode && !currentChannel._mode.getTopic()) {
        currentChannel._mode.setMode("t");
    } else if (!addMode && currentChannel._mode.getTopic()) {
        currentChannel._mode.unsetMode("t");
    }
}

int modePassword(Server &server, Client &user, Channel *currentChannel,
                 std::string password, bool addMode) {
    if (password.empty())
        return 411;
    if (addMode && password == currentChannel->getChannelKey()) {
        std::string keySet = server.message.getMessages(
            467, user, "", currentChannel->getChannelName());
        server.message.sendMessage(user, keySet);
        return 0;
    }
    if (addMode && !currentChannel->_mode.getKey()) {
        currentChannel->_mode.setMode("k");
        currentChannel->setChannelKey(password);
        std::string response = server.message.getMessages(324, user);

        response += currentChannel->getChannelName() + " +k " + password +"\r\n";
        // response += " set channel keyword to " + password + "\r\n ";
        currentChannel->broadcastMessage(response);
        return (0);
    } else if (!addMode && currentChannel->_mode.getKey()) {
        if (password != currentChannel->getChannelKey()) {
            std::string keySet = server.message.getMessages(
                467, user, "", currentChannel->getChannelName());
            server.message.sendMessage(user, keySet);
            return 1;
        }
        currentChannel->_mode.unsetMode("k");
        currentChannel->removeChannelKey();

        std::string response = server.message.getMessages(324, user);
        response += currentChannel->getChannelName() + " -k\r\n";
        // response += "Password was removed\r\n";
        currentChannel->broadcastMessage(response);
    }
    return (0);
}

int modeOperator(Server &server, Client &user, Channel *currentChannel,
                 std::string targetUserName, bool addMode) {
    std::string response;
    Client *targetUser = server.getClientByName(targetUserName);
    if (!targetUser || currentChannel->_fdUsersMap.find(targetUser->getFd()) ==
                           currentChannel->_fdUsersMap.end()) {
        response = server.message.getMessages(401, user);
        server.message.sendMessage(user, response);
        return 401;
    }
    if (addMode && !currentChannel->isUserOp(targetUserName)) {
        currentChannel->addOpUser(targetUserName);
        response = server.message.getMessages(324, user);
        response += currentChannel->getChannelName() + " +o " + targetUserName +
                    " \r\n";

        currentChannel->broadcastMessage(response);

    } else if (!addMode && currentChannel->isUserOp(targetUserName)) {
        currentChannel->removeOpUser(targetUserName);
        response = server.message.getMessages(324, user);
        response += currentChannel->getChannelName() + " -o " + targetUserName +
                    " \r\n";
        currentChannel->broadcastMessage(response);
    }
    return (0);
}

int modeLimit(Server &server, Client &user, Channel *currentChannel,
              std::string limitUser, bool addMode) {
    std::string response;
    if (addMode && currentChannel->_mode.getLimit())
        return 0;
    if (addMode && !currentChannel->_mode.getLimit()) {
        int limit = std::atoi(limitUser.c_str());
        if (limit > 0 && limit < INT_MAX) {
            currentChannel->setUserLimit(limit);
            currentChannel->_mode.setMode("l");
            std::string response = server.message.getMessages(324, user);
            response +=
                currentChannel->getChannelName() + " +l " + limitUser + "\r\n";
            currentChannel->broadcastMessage(response);
        }
    } else if (!addMode && currentChannel->_mode.getLimit()) {
        currentChannel->_mode.unsetMode("l");
        currentChannel->removeUserLimit();
        std::string response = server.message.getMessages(324, user);
        response += currentChannel->getChannelName() + " -l " + "\r\n";
        currentChannel->broadcastMessage(response);
    }
    return (0);
}

// END OF MODE FUNCTIONS

int modeFirstParse(Client &user, Server &server, Channel *currentChannel,
                   std::vector<std::string> &cmdSplittedSpace) {
    // check if user can change the modes (if is op)
    if (!currentChannel) {
        std::string response = server.message.getMessages(
            403, user, "", cmdSplittedSpace[1]); // 403: ERR_NOSUCHCHANNEL
        server.message.sendMessage(user, response);
        return (1);
    }

    if (cmdSplittedSpace.size() == 2) {
        std::string modes = currentChannel->_mode.getCurrentChannelMode();
        std::string response =
            server.message.getMessages(324, user); // 324: RPL_currentChannelEIS
        response += currentChannel->getChannelName() + " " + modes + "\r\n";
        server.message.sendMessage(user, response);
        return (1);
    }

    if (!currentChannel->isUserOp(user.getNickname())) {
        std::string response = server.message.getMessages(
            482, user, "", currentChannel->getChannelName());
        server.message.sendMessage(user, response);
        return (1);
    }

    // Send error message for non-existing channel
    if (cmdSplittedSpace[1] == server.getServerName()) {
        server.message.sendMessage(
            user, "You cannot change other user permisions.\r\n");
        return (1);
    }
    return (0);
}

int Command::cmdMode(Client &user, Server &server,
                     std::vector<std::string> &cmdSplittedSpace) {
    std::string response;
    std::string channelName = cmdSplittedSpace[1];
    Channel *currentChannel = server.getChannelByName(channelName);
    if (modeFirstParse(user, server, currentChannel, cmdSplittedSpace))
        return (1); // error, do something
    std::vector<std::string> params = cmdSplittedSpace;
    // shifteamos el inicio de cmdSplittedSpace y quitamos MODE y #channelName
    params.erase(params.begin(), params.begin() + 2);

    bool addMode = true;
    for (size_t j = 0; j < params.size(); j++) {
        std::string param = params[j];
        for (size_t i = 0; i < param.length(); ++i) {
            switch (params[j][i]) {
            case '+':
                addMode = true;
                break;
            case '-':
                addMode = false;
                break;
            case 'i':
                modeInvite(addMode, *currentChannel);
                response = message.getMessages(
                    324, user, "",
                    channelName + (addMode ? " +i" : " -i") + "\r\n");
                currentChannel->broadcastMessage(response);  
                break;
            case 't':
                modeTopic(addMode, *currentChannel);
                response = message.getMessages(
                    324, user, "",
                    channelName + (addMode ? " +t" : " -t") + "\r\n");

                currentChannel->broadcastMessage(response);
                break;
            case 'k':
                if (j + 1 < params.size() && !params[j + 1].empty()) {
                    modePassword(server, user, currentChannel, params[j + 1],
                                 addMode);
                    j++; // Nos saltamos el password
                } else {
                    response = server.message.getMessages(
                        461, user,
                        cmdSplittedSpace[0] + " " + cmdSplittedSpace[2]);
                    message.sendMessage(user, response);
                }
                break;
            case 'o':
                if (j + 1 < params.size() && !params[j + 1].empty()) {
                    modeOperator(server, user, currentChannel, params[j + 1],
                                 addMode);
                    j++;
                } else {
                    response = server.message.getMessages(
                        461, user,
                        cmdSplittedSpace[0] + " " + cmdSplittedSpace[2]);
                    message.sendMessage(user, response);
                }
                break;
            case 'l':
                if (addMode && j + 1 < params.size() &&
                    !params[j + 1].empty()) {
                    modeLimit(server, user, currentChannel, params[j + 1],
                              addMode);
                    j++;
                } else if (!addMode) {
                    modeLimit(server, user, currentChannel, params[j + 1],
                              addMode);
                } else {
                    response = server.message.getMessages(
                        461, user,
                        cmdSplittedSpace[0] + " " + cmdSplittedSpace[2]);
                    message.sendMessage(user, response);
                }

                break;
            default:
                char res = params[j][i];
                response = ":" + server.getServerName() + " 472 " + res +
                           " :is unknown mode char to me for " + channelName;
                message.sendMessage(user, response);
                break;
            }
        }
        response = "";
    }

    return (0);
}
