#include "Server.hpp"
#include <cstdlib>
#include <cstring>
#include <sys/types.h>

Server::Server(char *port, char *password) {
    p_port = port;
    p_password = password;
}
Server::~Server() {}

void Server::ListenAndServe() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cout << "Failed to create socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Listen to port 9999 on any address
    sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port =
        htons(std::atoi(p_port.c_str())); // htons is necessary to convert a
                                          // number to network byte order
    if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
        std::cout << "Failed to bind to port" << p_port << "." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Start listening. Hold at most 10 connections in the queue
    if (listen(sockfd, 10) < 0) {
        std::cout << "Failed to listen on socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Grab a connection from the queue
    uint16_t addrlen = sizeof(sockaddr);
    int connection =
        accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
    if (connection < 0) {
        std::cout << "Failed to grab connection." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Read from the connection
    char buffer[100];
    ssize_t bytesRead = read(connection, buffer, 100);
    std::cout << "The message was: " << buffer << " :" << bytesRead;

    // Send a message to the connection
    std::string response = "Good talking to you\n";
    send(connection, response.c_str(), response.size(), 0);
}
