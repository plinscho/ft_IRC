#include "../server/Command.hpp"
#include "../server/Server.hpp"
#include <iostream>

static int handlePart(Client &user, Server &server,
                      const std::string &channelName) {
    std::string response;
    Channel *currentChnl = server.getChannelByName(channelName);

    if (!currentChnl) {
        response = "Error. " + channelName + " does not exist.\r\n";
        server.message.sendMessage(user, response);
        return 0;
    }

    if (currentChnl->getChannelName() != channelName)
        return 1;
    response = ":" + user.getPrefix() + " PART " + channelName + "\r\n";
    currentChnl->broadcastMessage(response);
    currentChnl->removeUser(user.getFd());
    currentChnl->removeOpUser(user.getNickname());

    if (currentChnl->activeUsers == 0)
        server.deleteChannel(channelName);

    return 0;
}

int Command::cmdPart(Client &user, Server &server, std::string channelNames) {
    std::string response;

    std::vector<std::string> channelsPart =
        strTool.stringSplit(channelNames, ',');
    // Iterar sobre cada nombre de canal
    for (size_t i = 0; i < channelsPart.size(); ++i) {
        std::string channelName = channelsPart[i];
        // Manejar la salida del canal
        if (handlePart(user, server, channelName)) {
            std::string response =
                "Error. No such channel: " + channelName + "\r\n";
            message.sendMessage(user, response);
        }
    }
    return 0;
}
