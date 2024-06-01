#pragma once
#include <cerrno>
#include <cstdlib>      // For exit() and EXIT_FAILURE
#include <iostream>     // For cout
#include <netinet/in.h> // For sockaddr_in
#include <sys/socket.h> // For socket functions
#include <unistd.h>     // For read

class Server {
  public:
    Server(const char *, const char *);

    ~Server();

    void ListenAndServe();

  private:
    void createSocket();
    void bindSocket();
    void listenSockets();
    void handleConns();
    std::string p_port;
    std::string p_password;
    int sockfd;
};
