#include "Messages.hpp"

std::string Messages::getMessages(int code, const Client &client) {
    std::string nickname = client.getNickname();
    switch (code) {
    case 1:
        return "001 Welcome to the Internet Relay Network, " + nickname;
    case 2:
        return "002 Your host is <servername>, running version 42";
    case 3:
        return "003 This server was created <datetime>";
    case 4:
        return "004 <servername> <version> <available user modes> <available "
               "channel modes>";
    case 251:
        return "251 There are <integer> users and <integer> services on "
               "<integer> servers";
    case 252:
        return "252 <integer> operator(s) online";
    case 253:
        return "253 <integer> unknown connection(s)";
    case 254:
        return "254 <integer> channels formed";
    case 255:
        return "255 I have <integer> clients and <integer> servers";
    case 332:
        return "332 <channel> :<topic>";
    case 353:
        return "353 <nick> = <channel> :<names>";
    case 366:
        return "366 <channel> :End of NAMES list";
    case 401:
        return "401 " + nickname + " :No such nick/channel";
    case 403:
        return "403 <channel> :No such channel";
    case 404:
        return "404 <channel> :Cannot send to channel";
    case 411:
        return "411 :No recipient given (<command>)";
    case 412:
        return "412 :No text to send";
    case 421:
        return "421 <command> :Unknown command";
    case 422:
        return "422 :MOTD File is missing";
    default:
        return "Unknown code";
    }
}
