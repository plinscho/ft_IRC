#include "client/Client.hpp"
#include "server/Server.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>
#include <signal.h>



/* ROADMAP

	- Acabar handshake
	- Implementar NAMES (50 nicks por buffer de send)
	- Implementar JOIN
	- Implementar PART (OPCIONAL)

*/




bool power = true;

void sigHandler(int sigNum) {
	(void)sigNum;
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
	
	signal(SIGINT, sigHandler);

	while (power) 
		server.run();

	server.closeServer();
	return 0;
}
