#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[]) {

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <hostname> <port>\n";
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo *result;
    struct tm *tm;
    time_t t;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE; //Devolver 0.0.0.0
    hints.ai_family = AF_INET; //IPv4
    hints.ai_socktype = SOCK_DGRAM; //UDP

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);

    if (rc != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);

    if(sd == -1){
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    int bnd = bind(sd, result->ai_addr, result->ai_addrlen);

    if(bnd != 0){
        std::cerr << "Failed to bind" << std::endl;
        return -1;
    }

    struct addrinfo *i = result;
    bool running = true;

    while(running){
        char buffer[80];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr client;

        socklen_t clientLen = sizeof(struct sockaddr);

        int bytes = recvfrom(sd, (void *) &buffer, 80, 0, &client, &clientLen); 
        buffer[bytes] = '\0';

        if(bytes == -1){
            std::cerr << "Error receiving" << std::endl;
            return -1;
        }

        else{
            getnameinfo(&client, clientLen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
            std::cout << bytes << " bytes de " << host << ":" << serv << std::endl;

            char reBuffer[80];
            memset(reBuffer,0,80);
            int reSize;

            switch(buffer[0]){
            case 't':
                time(&t);
                tm = localtime(&t);
                reSize = strftime(reBuffer, sizeof(reBuffer), "%I:%M:%S %p", tm);
                sendto(sd, reBuffer, reSize, 0, (struct sockaddr *) &client, clientLen);
                break;
            case 'd':
                time(&t);
                tm = localtime(&t);
                reSize = strftime(reBuffer, sizeof(reBuffer), "%D", tm);
                sendto(sd, reBuffer, reSize, 0, (struct sockaddr *) &client, clientLen);
                break;
            case 'q':
                std::cout << "Saliendo..." << std::endl;
                running = false;
                break;
            default:
                std::cout << "Comando no soportado " << buffer << std::endl;
                break;
            }
        }

    }

    freeaddrinfo(result);
    close(sd);

    return 0;
}