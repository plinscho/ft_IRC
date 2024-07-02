#include "Messages.hpp"

std::string Messages::getMessages(int code, const Client &client) {
    std::string nickname = client.getNickname();
    switch (code) {
    case 1:
        return "001 Welcome to the Internet Relay Network, " + nickname + "\r\n";
    case 2:
        return "002 Your host is <servername>, running version 42\r\n";
    case 3:
        return "003 This server was created <datetime>\r\n";
    case 4:
        return "004 <servername> <version> <available user modes> <available channel modes>\r\n";
    case 251:
        return "251 There are <integer> users and <integer> services on <integer> servers\r\n";
    case 252:
        return "252 <integer> operator(s) online\r\n";
    case 253:
        return "253 <integer> unknown connection(s)\r\n";
    case 254:
        return "254 <integer> channels formed\r\n";
    case 255:
        return "255 I have <integer> clients and <integer> servers\r\n";
    case 332:
        return "332 <channel> :<topic>\r\n";
    case 353:
        return "353 <nick> = <channel> :<names>\r\n";
    case 366:
        return "366 <channel> :End of NAMES list\r\n";
    case 401:
        return "401 " + nickname + " :No such nick/channel\r\n";
    case 403:
        return "403 <channel> :No such channel\r\n";
    case 404:
        return "404 <channel> :Cannot send to channel\r\n";
    case 411:
        return "411 :No recipient given (<command>)\r\n";
    case 412:
        return "412 :No text to send\r\n";
    case 421:
        return "421 <command> :Unknown command\r\n";
    case 422:
        return "422 :MOTD File is missing\r\n";
    case 433:
        return "433 * " + nickname + " :Nickname is already in use\r\n";
    default:
        return "Unknown code\r\n";
    }
}
