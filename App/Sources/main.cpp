#include "../../Core/Server/Server.hpp"
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Error: use ./ircserv <port> <password>" << std::endl;

        return 1;
    }
    Server server(argv[1], argv[2]);

    server.ListenAndServe();

    return 0;
}
