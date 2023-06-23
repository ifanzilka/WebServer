#ifndef SOCKET42_HPP
# define SOCKET42_HPP

#include <string>
#include <iostream>

#include <sys/socket.h>	/* socket */
#include <unistd.h>     /* close */
#include "utils.hpp"    /* get_errno*/


class Socket
{
    public:
        
        /* Constructor */
        Socket();
        Socket(int domain, int type, int protocol);

        /* Set Flags Socket */
        void Setsockopt(int level, int option_name, const void *option_value, socklen_t option_len);

        
        /* Destructor */
        ~Socket();
    private:
        int _socket_fd;
        int _domain;
        int _type;
        int _protocol;
};

#endif



    //SERVER 1

    // int server_socket;
    // server_socket = socket(AF_INET, SOCK_STREAM, 0);
    // struct  sockaddr_in server_address;
    // struct  sockaddr_in client_address;

    // server_address.sin_family = AF_INET;           // IPv4
    // server_address.sin_port = htons(SERVER_PORT);
    
    // // port is 16 bit so htons is enough.
    // //htons returns host_uint16 converted to network byte order
    // server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    // // returns host_uint32 converted to network byte order

    // // bind the socket to our specified IP and port
    // // int bind(int sockfd, const struct  sockaddr *addr, socklen_t addrlen);
    // // sockfd argument is a file descriptor obtained from a previous call to socket()

    // // addr agrument is a pointer to a struckture specifying the address to which this socket is to be bound

    // rc = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));



    // КЛИЕНТ
	// int network_socket;
	// network_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	// // specify an address for the socket
	// struct sockaddr_in server_address;
	// server_address.sin_family = AF_INET;
	// server_address.sin_port = htons(9002);
	// server_address.sin_addr.s_addr = INADDR_ANY;

	// int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	// // check for error with the connection
	// if (connection_status == -1){
	// 	printf("There was an error making a connection to the remote socket \n\n");
	// }