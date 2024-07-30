#pragma once
#include "../client/Client.hpp"
#include <iostream>
#include <string>
#include <sstream>

class Client;
class Channel;

struct Messages {

    std::string getMessages(int, const Client &, std::string command = "", std::string channelName = "");
    int	        sendMessage(const Client &user, const std::string &msg);
    void        sendChannelNames(Channel &channel, Client &user);
    void	    sendWelcome(Client &user);


};
