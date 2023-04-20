#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>

using namespace std;

int main(int argc, char** argv){

	if(argc != 4){
		cerr << "Usage: " << argv[0] << " <dir> <port> <command>" << endl;
	    return -1;
	}

	struct addrinfo hints;
	struct addrinfo* result;

	memset(&hints,0,sizeof(struct addrinfo));

	hints.ai_family = AF_INET; //IPv4
	hints.ai_socktype = SOCK_DGRAM; //UDP

	int rc = getaddrinfo(argv[1], argv[2], &hints, &result);

    if (rc != 0) {
        cerr << "getaddrinfo: " << gai_strerror(rc) << endl;
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);

    if(sd == -1){
        cerr << "Failed to create socket" << endl;
        return -1;
	}

	int rsend = sendto(sd, argv[3], 2, 0, result->ai_addr, result->ai_addrlen);

	if(rsend == -1){
		cerr << "Error sending msg" << endl;
		return -1;
	}

	else if( rsend < 1){
		cerr << "Not all bytes were sent" << endl;
	}

	freeaddrinfo(result);

	char buffer[80];
	memset(buffer, 0, 80);
	int rbytes = recvfrom(sd, (void*)buffer, 80, 0, NULL, NULL);
	if(rbytes == -1){
		cerr << "Error receiving" << endl;
	}

	cout << buffer;

    return 0;
}