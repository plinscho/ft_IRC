#pragma once
#include "../client/Client.hpp"
#include <iostream>
#include <string>
#include <sstream>

class Client;

struct Messages {

    std::string getMessages(int, const Client &, std::string command = "");
    int	        sendMessage(Client *user, const std::string &msg);

};
