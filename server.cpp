#include<iostream>
#include <netinet/in.h> // Supersets socket.h
#include <netdb.h> // Defines addrinfo struct
#include <arpa/inet.h> // For inet_ntop()
#include <errno.h> // For retreiving and interpreting error numbers
#include <string.h>

using namespace std;

int main(){
    cout << "hello world" << endl;
    // struct that configures our network connection
    struct addrinfo hints;
    /*
    AI_PASSIVE means the address will have a socket bound to it (server use case). getaddrinfo() will select 0.0.0.0 if node param is NULL. 
    For client use case, node would be a domain name or IP address and ai_flags would be AI_CANONICAL/AI_NUMERIC respectively. 
    For localhost, set both ai_flags and node param to NULL 
    */ 
    hints.ai_flags = AI_PASSIVE; 
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // Streaming socket for TCP
    hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol

    struct addrinfo *serverinfo; // Pointer for results
    int status = getaddrinfo(NULL, "8080", &hints, &serverinfo);
    if (status != 0){
        cout << "error creating struct: " << gai_strerror(status) << endl;
    }
    //print socketaddr structure
    for (struct addrinfo *p = serverinfo; p != NULL; p = p->ai_next){
        struct sockaddr_in *sock_ipv4 = (struct sockaddr_in*)p->ai_addr; // cast sockaddr to sockaddr_in, as its ipv4 specific
        char ip_address_string[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sock_ipv4->sin_addr), ip_address_string, sizeof(ip_address_string)); // Convert IP to string and store inside 
        cout << "IP Address: " << ip_address_string << endl;
        cout << "Port Number: " << ntohs(sock_ipv4->sin_port)<< endl; // port number is stored in network byte order (big endian), but we need to convert it to host byte order (which depends on our system)
    }
    // create socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    // bind to the socket
    // not needed in the client case, the kernel will randomly assign us a local port number to which the servers response will be routed. Only call bind() for the server case
    status = bind(socket_fd, serverinfo->ai_addr, serverinfo->ai_addrlen);
    if (status == -1){
        cout << "Error binding to socket: " << strerror(errno) << endl;
    }

    // start listening on the socket
    int pending_connection_queue_size = 2;
    status = listen(socket_fd, pending_connection_queue_size);
    if (status == -1){
        cout << "Error listening to socket: " << strerror(errno) << endl;
    }

    return 0;
};