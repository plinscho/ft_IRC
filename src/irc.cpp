#include "client/Client.hpp"
#include "server/Server.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>
#include <signal.h>

bool power = true;



void sigHandler(int sigNum) {
	std::cout << "Caught signal " << sigNum << std::endl;
	power = false;	
}

int main(int argc, char **argv) {
	if (argc != 3)
		return (quickError("Error.\n Use: ./ircserv <port> <password>", EXIT_FAILURE));

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
	server.initServer();
	
	signal(SIGINT, sigHandler);
	
	while (power){
		server.run();
		if (!power)
			break;
	}

	server.closeServer();
	return 0;
}
