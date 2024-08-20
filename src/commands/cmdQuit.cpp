#include "../server/Command.hpp"
#include "../server/Server.hpp"
#include <iostream>

int Command::cmdQuit(Server &server, Client &user, std::string reason) {
    std::string response;
    if (reason.empty())
        reason = "Leaving...";

    std::vector<std::string>::iterator i = user.channelsJoined.begin();
    for (; i != user.channelsJoined.end(); i++) {
        Channel *chan = server.getChannelByName(*i);
        if (chan) {
            response = ":" + user.getPrefix() + " PART " + *i + "\r\n";
            chan->broadcastMessage(response);
            chan->removeUser(user.getFd());
        }
    }
    return 1;
}
