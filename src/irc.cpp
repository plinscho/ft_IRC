#include "client/Client.hpp"
#include "server/Server.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>
#include <signal.h>

bool power = true;

void sigHandler(int sigNum) {
    (void)sigNum;
    power = false;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Error: use ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    int port = atoi(argv[1]);

    if (port < 1024 || port > 49151) {
        std::cout << "Wrong port!" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string password = argv[2];
    if (password.empty()) {
        std::cerr << "Error: usage is " << argv[0] << " <" << argv[1] << "> <"
                  << argv[2] << ">" << std::endl;
        exit(EXIT_FAILURE);
    }
    Server server(port, argv[2]);
    server.initPoll();

    signal(SIGINT, sigHandler);

    while (power) 
    {
		server.grabConnection();
        server.run();
    }
    server.closeSockets();
    return 0;
}
