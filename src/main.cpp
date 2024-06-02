#include "../include/Server.hpp"
#include <iostream>
#include <signal.h>

bool	powerOn = true;

void	sigHandler(int sigNum)
{
	(void)sigNum;
	powerOn = false;
}

int main(int argc, char **argv) 
{
    if (argc != 3) 
	{
        std::cout << "Error: use ./ircserv <port> <password>" << std::endl;
        return 1;
    }

	int port = atoi(argv[1]);

	if (port < 1024 || port > 49151)
	{
		std::cout << "Wrong port!" << std::endl;
		exit(EXIT_FAILURE);
	}

    Server server(port, argv[2]);
	server.createSocket();
	server.bindSocket();
	server.listenSockets();

	signal(SIGINT, sigHandler);
	while (powerOn)
	{
		server.handleConns();
	}
    return 0;
}
