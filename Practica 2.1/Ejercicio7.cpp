#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <hostname> <port>" << std::endl;
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_flags = AI_PASSIVE; //Devolver 0.0.0.0
    hints.ai_family = AF_INET; //IPv4
    hints.ai_socktype = SOCK_STREAM;//TCP

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

    int lis = listen(sd, 16);
	if(lis == -1){
		std::cerr << "Listen failed" << std::endl;
        return -1;
	}

    struct sockaddr client;
    socklen_t clientLen = sizeof(struct sockaddr);

    int client_sd = accept(sd, &client, &clientLen);
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo(&client, clientLen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

    std::cout << "Conexión desde " << host << " " << serv << std::endl;

    bool running = true;

    while(running){
        char buffer[80];

        memset(buffer, 0, 80);

        int bytes = recv(client_sd, (void*)buffer, 80, 0);

        if(bytes <= 0){
            std::cerr << "Conexión terminada" << std::endl;
            running = false;
            return -1;
        }

        else {
            buffer[bytes] = '\0';
            send(client_sd, buffer, bytes, 0);
        }

    }

    freeaddrinfo(result);
    close(sd);

    return 0;
}