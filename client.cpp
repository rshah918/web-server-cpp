#include<iostream>
#include <netinet/in.h> // Supersets socket.h
#include <netdb.h> // Defines addrinfo struct
#include <arpa/inet.h> // For inet_ntop()
#include <errno.h> // For retreiving and interpreting error numbers
#include <string.h>
#include <unistd.h>


using namespace std;

int main(){
    cout << "Starting Client..." << endl;
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

    struct addrinfo *clientinfo; // Pointer for results
    int status = getaddrinfo(NULL, "8080", &hints, &clientinfo);
    if (status != 0){
        cout << "error creating struct: " << gai_strerror(status) << endl;
    }
    //print socketaddr structure
    for (struct addrinfo *p = clientinfo; p != NULL; p = p->ai_next){
        struct sockaddr_in *sock_ipv4 = (struct sockaddr_in*)p->ai_addr; // cast sockaddr to sockaddr_in, as its ipv4 specific
        char ip_address_string[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sock_ipv4->sin_addr), ip_address_string, sizeof(ip_address_string)); // Convert IP to string and store inside 
        cout << "IP Address: " << ip_address_string << endl;
        cout << "Port Number: " << ntohs(sock_ipv4->sin_port)<< endl; // port number is stored in network byte order (big endian), but we need to convert it to host byte order (which depends on our system)
    }
    // create socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // connect to remote process
    status = connect(socket_fd, clientinfo->ai_addr, clientinfo->ai_addrlen);
    if (status == -1) {
        cout << "Error connecting to server: " << strerror(errno) << endl;
    }
    // spam the hell out of the server with a bunch of hello worlds
    while(1){
        struct sockaddr client_connection;
        unsigned int size_client_connection = sizeof client_connection;
        string message = "hello world";
        unsigned int bytes_sent = send(socket_fd, &message, 11, 0);
        if (bytes_sent == -1){
            cout << "Error sending message: " << strerror(errno) << endl;
        }
        else{
            cout << bytes_sent << " Bytes sent" << endl;
        }
    }
    //close socket and addrinfo data structure
    close(socket_fd);
    freeaddrinfo(clientinfo);
    return 0;
};