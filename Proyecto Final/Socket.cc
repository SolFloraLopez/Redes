#include <string.h>

#include "Serializable.h"
#include "Socket.h"

using namespace std;
Socket::Socket(const char * addresults, const char * port):sd(-1)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    struct addrinfo* result;

    int sc = getaddrinfo(addresults, port, &hints, &result);
    if(sc != 0){
        cerr << "[getaddrinfo]: " << gai_strerror(sc) << "\n";
        throw ("Error en getaddrinfo: " + string(gai_strerror(sc)));
    }

    sd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if(sd < 0){
        cout << "Error en la creacion del socket, cerrando el programa... Código de error: " << errno << "\n";
        freeaddrinfo(result);
        throw "Error al abrir el socket, código de error: ";
    }

    sa_len = result->ai_addrlen;
    sa = *result->ai_addr;
}

int Socket::recv(Serializable &obj, Socket * &sock)
{
    struct sockaddr sa;
    socklen_t sa_len = sizeof(struct sockaddr);

    char buffer[MAX_MESSAGE_SIZE];

    ssize_t bytes = ::recvfrom(sd, buffer, MAX_MESSAGE_SIZE, 0, &sa, &sa_len);

    if ( bytes <= 0 )
    {
        return -1;
    }

    if ( sock != 0 )
    {
        sock = new Socket(&sa, sa_len);
    }

    obj.from_bin(buffer);

    return 0;
}

int Socket::send(Serializable& obj, const Socket& sock)
{
    //Serializar el objeto
    obj.to_bin();
    //Enviar el objeto binario a sock usando el socket sd
    int rc = sendto(sd, obj.data(), obj.size(), 0, &sock.sa, sock.sa_len);
    return rc;
}

bool operator== (const Socket &s1, const Socket &s2)
{
    //Comparar los campos sin_family, sin_addr.s_addr y sin_port
    //de la estructura sockaddr_in de los Sockets s1 y s2
    sockaddr_in *in1 = (sockaddr_in*) &s1.sa;
    sockaddr_in *in2 = (sockaddr_in*) &s2.sa;

    //Retornar false si alguno difiere
    return (in1->sin_family == in2->sin_family && in1->sin_addr.s_addr == in2->sin_addr.s_addr 
        && in1->sin_port == in2->sin_port);
};

std::ostream& operator<<(std::ostream& os, const Socket& s)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo((struct sockaddr *) &(s.sa), s.sa_len, host, NI_MAXHOST, serv,
                NI_MAXSERV, NI_NUMERICHOST);

    os << host << ":" << serv;

    return os;
};
