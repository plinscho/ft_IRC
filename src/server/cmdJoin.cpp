#include "Command.hpp"
#include "Server.hpp"
#include <iostream>

static void createNewChannel(Client &user, Server &server, const std::string &channelName) {
    std::string response;

    // Create new channel
    Channel *newChannel = new Channel(user.getFd(), channelName);
    server._channels[user.getFd()] = newChannel;
    newChannel->addUser(user.getFd(), user);

    // The first member becomes an operator
    newChannel->nickOp.push_back(user.getNickname());

    // Notify all users in the channel about the new user
    response = ":" + user.getPrefix() + " JOIN " + channelName + "\r\n";
    newChannel->broadcastMessage(response);

    // Send the channel names
    server.message.sendChannelNames(*newChannel, user);
}

static void handleExistingChannel(Client &user, Server &server, Channel *channel, const std::string &channelName, const std::string &password) {
    std::string response;

    // Verify if the user is invited if the channel is in invite-only mode
    if (channel->_mode.getCurrentChannelMode().find('i') != std::string::npos) {
        if (!channel->isInvited(user.getNickname())) {
            response = "Error. You must be invited to join the channel " + channelName + "\r\n";
            server.message.sendMessage(user, response);
            return;
        }
    }

    // Verify password
    if (channel->_mode.getCurrentChannelMode().find('k') != std::string::npos) {
        if (channel->getPassword() != password) {
            response = "Error. Incorrect password for channel " + channelName + "\r\n";
            server.message.sendMessage(user, response);
            return;
        }
    }

    channel->addUser(user.getFd(), user);

    // Notify all users in the channel about the new user
    response = ":" + user.getPrefix() + " JOIN " + channelName + "\r\n";
    channel->broadcastMessage(response);

    // Send the channel topic if it exists
    std::string topic = channel->getTopic();
    if (!topic.empty()) {
        response = ":irc.middleman.org 332 " + user.getNickname() + " " + channelName + " :" + topic + "\r\n";
        server.message.sendMessage(user, response);
    }

    // Send the channel names
    server.message.sendChannelNames(*channel, user);
}

static bool handleJoinChannel(Client &user, Server &server, const std::string &channelName, const std::string &password) {
    // Check if the channel name is empty
    if (channelName.empty()) {
        std::string response = server.message.getMessages(461, user);
        server.message.sendMessage(user, response);
        return false;
    }

    // Find if the channel already exists
    std::map<int, Channel*>::iterator it = server._channels.begin();
    while (it != server._channels.end()) {
        if (it->second->getChannelName() == channelName) {
            handleExistingChannel(user, server, it->second, channelName, password);
            return true;
        }
        ++it;
    }

    // If channel was not found, create a new one
    if (channelName[0] != '#') {
        std::string response = "Error. Channel name must start with #\r\n";
        server.message.sendMessage(user, response);
        return false;
    }

    createNewChannel(user, server, channelName);
    return true;
}

int Command::cmdJoin(Client &user, Server &server, std::string cmd) {
    std::vector<std::string> cmdSplittedSpace = strTool.stringSplit(cmd, ' ');

    // Check if the command is empty or malformed
    if (cmdSplittedSpace.size() < 2) {
        std::string response = message.getMessages(461, user);
        message.sendMessage(user, response);
        return (0);
    }

    std::string channelsStr = cmdSplittedSpace[1]; 
    std::string passwordsStr = cmdSplittedSpace.size() > 2 ? cmdSplittedSpace[2] : "";

    // Split the channel names and passwords by comma
    std::vector<std::string> channelsJoin = strTool.stringSplit(channelsStr, ',');
    std::vector<std::string> passwords = strTool.stringSplit(passwordsStr, ',');

    // Iterate over each channel name
    for (size_t i = 0; i < channelsJoin.size(); ++i) {
        std::string channelName = channelsJoin[i];
        std::string password = i < passwords.size() ? passwords[i] : "";

        // Handle joining the channel
        if (!handleJoinChannel(user, server, channelName, password)) {
            std::string response = "Error. No such channel: " + channelName + "\r\n";
            message.sendMessage(user, response);
        }
    }
    return (0);
}