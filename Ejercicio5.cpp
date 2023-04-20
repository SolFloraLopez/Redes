#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <vector>
#include <stdlib.h>

int main(int argc, char** argv) {

    if (argc != 3) {
        std::cerr << "usage " << argv[0] << "<dir> <port>\n";
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo* result;

    std::memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE; //Devolver 0.0.0.0
    hints.ai_family = AF_INET; //IPv4
    hints.ai_socktype = SOCK_STREAM; //TCP

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);

    if (rc != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(rc) << "\n";
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);

    if (sd == -1) {
        std::cerr << "Failed to create socket\n";
        return -1;
    }

    int rcon = connect(sd, result->ai_addr, result->ai_addrlen);

    if (rcon != 0) {
        std::cerr << "Connection failed\n";
        return -1;
    }

    bool running = true;

    while (running) {
        char buffer[80];
        std::memset(buffer, 0, 80);

        std::cin >> buffer;

        int len = std::strlen(buffer);
        buffer[len] = '\0';

        if (len == 1 && buffer[0] == 'Q') {
            running = false;
            break;
        }

        int bSent = send(sd, buffer, len, 0);

        std::vector<char> recv_buffer(len);
        recv(sd, &recv_buffer[0], len, 0);

        std::cout << &recv_buffer[0] << std::endl;
    }

    freeaddrinfo(result);
    close(sd);

    return 0;
}