#include <iostream>
#include "Client.hpp"

Client::Client(const char *port, const char *password) {
	p_port = port;
	p_password = password;
	sockfd = -1;
}

Client::~Client() {
	if (sockfd != -1) {
		close(sockfd);
	}
}


