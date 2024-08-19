#include "../server/Command.hpp"
#include "../server/Server.hpp"
#include <iostream>

// COMANDO hecho por HexChat: KICK <nombreCanal> <user>
int Command::cmdKick(Client &user, Server &server, std::string channelName,
                     std::string target) {
    std::string response;
    Channel *currentChnl = server.getChannelByName(channelName);

    if (!currentChnl) {
        response = "Error. " + channelName + " does not exist.\r\n";
        message.sendMessage(user, response);
        return (0);
    }

    if (currentChnl->getChannelName() != channelName)
        return 1;
    if (!currentChnl->isUserOp(user.getNickname())) {
        response =
            message.getMessages(482, user, "", currentChnl->getChannelName());
        message.sendMessage(user, response);
        return 0;
    }
    Client *targetRef = server.getClientByName(target);
    if (!targetRef)
        return 1;
    if (targetRef->getNickname() == target) {
        response = ":" + user.getPrefix() + " KICK " + channelName + " " +
                   target + "\r\n";
        currentChnl->broadcastMessage(response);
        currentChnl->removeUser(targetRef->getFd());
        currentChnl->removeOpUser(target);
        targetRef->eraseJoinedChannel(channelName);
        return 0;
    }
    response = message.getMessages(401, user);
    message.sendMessage(user, response);
    return (0);
}
