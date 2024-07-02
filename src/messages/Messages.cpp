#include "Messages.hpp"

std::string Messages::getMessages(int code, const Client &client) {
        std::string nickname = client.getNickname();
        std::string username = client.getUsername(); // Supongamos que se obtiene el username
        std::string serverName = "irc.middleman.net"; // Nombre del servidor
        std::string serverVersion = "42"; // Versión del servidor
        std::string datetime = "01-01-2024 12:00"; // Fecha y hora de creación del servidor

        switch (code) {
            case 1:
                return ":" + serverName + " 001 " + nickname + " :Welcome to the Internet Relay Network, " + nickname + "\r\n";
            case 2:
                return ":" + serverName + " 002 " + nickname + " :Your host is " + serverName + ", running version " + serverVersion + "\r\n";
            case 3:
                return ":" + serverName + " 003 " + nickname + " :This server was created " + datetime + "\r\n";
            case 4:
                return ":" + serverName + " 004 " + nickname + " " + serverName + " " + serverVersion + " <available user modes> <available channel modes>\r\n";
            case 251:
                return ":" + serverName + " 251 " + nickname + " :There are <integer> users and <integer> services on <integer> servers\r\n";
            case 252:
                return ":" + serverName + " 252 " + nickname + " :<integer> operator(s) online\r\n";
            case 253:
                return ":" + serverName + " 253 " + nickname + " :<integer> unknown connection(s)\r\n";
            case 254:
                return ":" + serverName + " 254 " + nickname + " :<integer> channels formed\r\n";
            case 255:
                return ":" + serverName + " 255 " + nickname + " :I have <integer> clients and <integer> servers\r\n";
            case 332:
                return ":" + serverName + " 332 " + nickname + " <channel> :<topic>\r\n";
            case 353:
                return ":" + serverName + " 353 " + nickname + " = <channel> :<names>\r\n";
            case 366:
                return ":" + serverName + " 366 " + nickname + " <channel> :End of NAMES list\r\n";
            case 401:
                return ":" + serverName + " 401 " + nickname + " :No such nick/channel\r\n";
            case 403:
                return ":" + serverName + " 403 " + nickname + " <channel> :No such channel\r\n";
            case 404:
                return ":" + serverName + " 404 " + nickname + " <channel> :Cannot send to channel\r\n";
            case 411:
                return ":" + serverName + " 411 " + nickname + " :No recipient given (<command>)\r\n";
            case 412:
                return ":" + serverName + " 412 " + nickname + " :No text to send\r\n";
            case 421:
                return ":" + serverName + " 421 " + nickname + " <command> :Unknown command\r\n";
            case 422:
                return ":" + serverName + " 422 " + nickname + " :MOTD File is missing\r\n";
            case 433:
                return ":" + serverName + " 433 " + username + " " + nickname + " :Nickname is already in use\r\n";
            case 464:
                return ":" + serverName + " 464 " + nickname + " :Password required or incorrect\r\n";
            default:
                return "Unknown code\r\n";
        }
}