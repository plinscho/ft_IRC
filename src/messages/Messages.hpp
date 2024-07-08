#pragma once
#include "../client/Client.hpp"
#include <iostream>
#include <string>
#include <sstream>

class Client;
struct Messages {

    std::string getMessages(int, const Client &, std::string command = "");
};
