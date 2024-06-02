#include "Server.hpp"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/types.h>

Server::Server(int port, char *password) {
    _port = port;
    p_password = password;
    sockfd = -1;
}
Server::~Server() {
    if (sockfd != -1) {
        close(sockfd);
    }
}

void Server::createSocket() {

    //	int socket(int domain, int type, int protocol); from man socket
    //	AF_INET = IPv4 Internet protocols, type SOCK_STREAM. 0 is the default
    // protocol

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        exit(EXIT_FAILURE);
    }
}

// The bind() function assigns a local protocol address to a socket.
// In the context of internet-based sockets, this address is a combination
// of an IP address and a port number.
void Server::bindSocket() {

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY; // man 7 ip
    sockaddr.sin_port = htons(_port);
    if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
        std::cout << "Failed to bind to port" << p_port << "." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::listenSockets() {
    // listen up to 10 connections from the socket fd
    if (listen(sockfd, 10) < 0) {
        std::cout << "Failed to listen on socket." << std::endl;
        exit(EXIT_FAILURE);
    }
    /*

F_SETFL (int)
Set the file status flags to the value specified by arg.  File access mode
(O_RDONLY, O_WRONLY, O_RDWR) and  file creation flags (i.e., O_CREAT, O_EXCL,
O_NOCTTY, O_TRUNC) in arg are ignored.  On Linux, this command can change only
the O_APPEND, O_ASYNC, O_DIRECT, O_NOATIME, and O_NONBLOCK flags.   It  is  not
possible to change the O_DSYNC and O_SYNC flags; see BUGS, below.

                            */
    // fcntl(sockfd, F_SETFL, O_NONBLOCK);
}

/*
 The accept() function is used with connection-based socket types (SOCK_STREAM,
 SOCK_SEQPACKET). It extracts the first connection request on the queue of
 pending connections for the listening socket, sockfd, creates a new connected
 socket, and returns a new file descriptor referring to that socket. The newly
 created socket is not in the listening state. The original socket sockfd is
 unaffected by this call. The argument addr is a pointer to a sockaddr
 structure. This structure is filled in with the address of the peer socket, as
 known to the communications layer. The exact format of the address, as returned
 by accept(), depends on the communications domain in which the socket is
 created. The addrlen argument is a value-result argument: it should initially
 contain the size of the structure pointed to by addr, on return it will contain
 the actual size of the peer address.
*/
void Server::handleConns() {
    sockaddr_in sockaddr; // This structure will hold the address of the client
                          // that is connecting to the server.

    // Grab a connection from the queue
    uint16_t addrlen = sizeof(sockaddr);

    //	Every time a new client connects to the server, the accept() function
    // creates 	a new socket and returns a new file descriptor for that socket.
    //	This new file descriptor is used for communication with that specific
    // client.
    while (1) {
        int connection =
            accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
        if (connection < 0) {
            // throw("in connection");
            std::cout << "aqui peta>????" << std::endl;
        }

        std::string ip = inet_ntoa(sockaddr.sin_addr);

        // Read from the connection
        char buffer[1024];
        ssize_t bytesRead = read(connection, buffer, 100);
        (void)bytesRead;
        std::cout << "nickname@" << ip << ": " << buffer << std::endl;
        memset(buffer, 0, 1024);
        // Send a message to the connection
        // std::string response = "Good talking to you\n";
        // send(connection, response.c_str(), response.size(), 0);
    }
}

void Server::ListenAndServe() {
    createSocket();
    bindSocket();
    listenSockets();
    handleConns();
}