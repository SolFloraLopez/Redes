#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <netdb.h>
#include <string.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <hostname>\n";
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;

    int rc = getaddrinfo(argv[1], NULL, &hints, &result);

    if (rc != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    struct addrinfo *i = result;

    while(i != NULL){

        char host[NI_MAXHOST];

        getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, 0, 0, NI_NUMERICHOST);

        std::cout << host << " " << i->ai_family << " " << i->ai_socktype << std::endl;

        i = i->ai_next;
    }

    freeaddrinfo(result);

    return 0;
}