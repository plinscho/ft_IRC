#include "client/Client.hpp"
#include "server/Server.hpp"
#include <exception>
#include <iostream>
#include <signal.h>

bool powerOn = true;

void sigHandler(int sigNum) {
    (void)sigNum;
    powerOn = false;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Error: use ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    int port = atoi(argv[1]);

    if (port < 1024 || port > 49151) {
        std::cout << "Wrong port!" << std::endl;
        exit(EXIT_FAILURE);
    }
    //    try {
    Server server(port, argv[2]);
    server.createSocket();
    server.bindSocket();
    server.listenSockets();
    // while (powerOn) {
    //  server.listenSockets();

    signal(SIGINT, sigHandler);
    // server.handleConns();
    //  }
    //  } catch (std::exception &e) {
    //    std::cerr << "Server FAILURE: " << e.what() << std::endl;
    //}

	while (powerOn)
	{
		server.getConnections();
	}
    return 0;
}
