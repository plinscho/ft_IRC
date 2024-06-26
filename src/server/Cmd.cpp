#include "../client/Client.hpp"
#include "Server.hpp"
#include <iostream>
#include <vector>

int cmdLogin(std::vector<std::string> cmd, Client *user)
{
    if (cmd.empty() || !user)
        return (1);
    return (0);
}

int cmdJoin(std::vector<std::string> cmd, Client *user)
{
    if (cmd.empty() || !user)
        return (1);
    return (0);
}

int cmdSetNick(std::vector<std::string> cmd, Client *user)
{
    if (cmd.empty() || !user)
        return (1);

    return (0);
}

int cmdSetUname(std::vector<std::string> cmd, Client *user)
{
    if (cmd.empty() || !user)
        return (1);

    return (0);
}

int cmdSend(std::vector<std::string> cmd, Client *user)
{
    if (cmd.empty() || !user)
        return (1);

    return (0);
}

int cmdHelp(std::vector<std::string> cmd, Client *user)
{
    if (cmd.empty() || !user)
        return (1);
    if (user->getFd() == -1)
        return (1);
    return (sendWelcome(user->getFd()));
}
