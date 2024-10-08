#include "Messages.hpp"
#include "../server/Channel.hpp"

// https://www.alien.net.au/irc/irc2numerics.html

int Messages::sendMessage(const Client &user, const std::string &msg) {
    if (msg.empty())
        return (-1);
    if (send(user.getFd(), msg.c_str(), msg.length(), MSG_DONTWAIT) == -1)
        return (-1);
    return (1);
}

void Messages::sendWelcome(Client &user) {
    std::string response;
    response = getMessages(1, user);
    sendMessage(user, response);
    response = getMessages(2, user);
    sendMessage(user, response);
    response = getMessages(3, user);
    sendMessage(user, response);
    response = getMessages(4, user);
    sendMessage(user, response);
}

void Messages::sendChannelNames(Channel &channel, Client &user) {
    std::string response;
    std::vector<std::string> channelMembers = channel.getChannelsNicks();
    std::vector<std::string>::iterator it;

    // Formato del mensaje RPL_NAMREPLY
    response = ":Middleman 353 " + user.getNickname() + " = " +
               channel.getChannelName() + " :";
    for (it = channelMembers.begin(); it != channelMembers.end(); ++it) {
        response += *it + " ";
    }
    response += "\r\n";
    sendMessage(user, response);

    // Enviar RPL_ENDOFNAMES
    response = ":irc.middleman.org 366 " + user.getNickname() + " " +
               channel.getChannelName() + " :End of NAMES list\r\n";
    sendMessage(user, response);
}

std::string Messages::getMessages(int code, const Client &client,
                                  std::string command,
                                  std::string channelName) {
    std::string nickname = client.getNickname();
    std::string username =
        client.getUsername(); // Supongamos que se obtiene el username
    std::string serverName = "Middleman"; // Nombre del servidor
    std::string serverVersion = "42";     // Versión del servidor
    std::string datetime =
        "01-01-2024 12:00"; // Fecha y hora de creación del servidor

    switch (code) {
    case 1:
        return ":" + serverName + " 001 " + nickname +
               " :Welcome to the Middleman Relay Network, " + nickname + "\r\n";
    case 2:
        return ":" + serverName + " 002 " + nickname + " :Your host is " +
               serverName + ", running version " + serverVersion + "\r\n";
    case 3:
        return ":" + serverName + " 003 " + nickname +
               " :This server was created " + datetime + "\r\n";
    case 4:
        return ":" + serverName + " 004 " + nickname + " " + serverName + " " +
               serverVersion + "\r\n";
    case 324:
        return ":" + serverName + " 324 " + nickname + " " + channelName;
    case 332:
        return ":" + serverName + " 332 " + nickname +
               " <channel> :<topic>\r\n";
    case 353:
        return ":" + serverName + " 353 " + nickname +
               " = <channel> :<names>\r\n";
    case 366:
        return ":" + serverName + " 366 " + nickname +
               " <channel> :End of NAMES list\r\n";
    case 401:
        return ":" + serverName + " 401 " + nickname +
               " :No such nick/channel\r\n";
    case 403:
        return ":" + serverName + " 403 " + nickname + " " + channelName +
               " :No such channel\r\n";
    case 404:
        return ":" + serverName + " 404 " + nickname +
               " <channel> :Cannot send to channel\r\n";
    case 411:
        return ":" + serverName + " 411 " + nickname +
               " :No recipient given (<command>)\r\n";
    case 412:
        return ":" + serverName + " 412 " + nickname + " :No text to send\r\n";
    case 421:
        return ":" + serverName + " 421 " + nickname +
               " <command> :Unknown command\r\n";
    case 422:
        return ":" + serverName + " 422 " + nickname +
               " :MOTD File is missing\r\n";
    case 433:
        return ":" + serverName + " 433 " + username + " " + nickname +
               " :Nickname is already in use\r\n";
    case 444:
        return ":" + serverName + " 444 " + username + " " + nickname +
               " Error: User is not logged in.\r\n";
    case 461:
        return ":" + serverName + " 461 " + command +
               " :Not enough parameters\r\n";
    case 464:
        return ":" + serverName + " 464 " + nickname +
               " :Password required or incorrect\r\n";
    case 467:
        return ":" + serverName + " 467 " + channelName +
               " :Channel key already set\r\n";
    case 471:
        return ":" + serverName + " 471 " + nickname + " " + channelName +
               " :Channel is full(+l)\r\n";
    case 473: return ":" + serverName + " 473 " + channelName + " :Cannot join channel (+i)\r\n";
    case 475: return ":" + serverName + "475 " + nickname + " " + channelName + " :Cannot join channel (+k)";
    case 482:
        return ":" + serverName + " 482 " + nickname + " " + channelName +
               " : You're not channel operator\r\n";

        // ########################### CODIGOS PROPIOS DE NUESTRO SERVIDOR - NO
        // NUMERICO
    case 1001:
        return ":" + command + " " + nickname + " " + command + " ";
    default:
        return "Unknown code\r\n";
    }
}
