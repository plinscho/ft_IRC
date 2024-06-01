#include "Server.hpp"
#include <cstdlib>
#include <cstring>
#include <sys/types.h>

Server::Server(const char *port, const char *password) {
    p_port = port;
    p_password = password;
    sockfd = -1;
}
Server::~Server() {
    if (sockfd != -1) {
        close(sockfd);
    }
}

void Server::createSocket() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket." << std::endl;
    }
}

void Server::bindSocket() {

    sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port = htons(std::atoi(p_port.c_str()));
    if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
        std::cout << "Failed to bind to port" << p_port << "." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::listenSockets() {
    if (listen(sockfd, 10) < 0) {
        std::cout << "Failed to listen on socket." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::handleConns() {
    sockaddr_in sockaddr; // maybe to make do this on the class

    // Grab a connection from the queue
    uint16_t addrlen = sizeof(sockaddr);
    int connection =
        accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
    if (connection < 0) {
        std::cout << "Failed to grab connection." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Read from the connection
    char buffer[1024];
    ssize_t bytesRead = read(connection, buffer, 100);
    std::cout << "The message was: " << buffer << " :" << bytesRead;

    // Send a message to the connection
    std::string response = "Good talking to you\n";
    send(connection, response.c_str(), response.size(), 0);
}

void Server::ListenAndServe() {
    createSocket();
    bindSocket();
    listenSockets();
    handleConns();
}
