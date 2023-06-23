#include "Socket.hpp"


/* Constructos */
Socket::Socket()
{
    _socket_fd = -1;
    _domain = -1;
    _type = -1;
    _protocol = -1;
}

Socket::Socket(int domain, int type, int protocol)
{
    _socket_fd = socket(domain, type, protocol);
    if (_socket_fd == -1)
    {
        throw std::runtime_error("Create socket Error:\n" +  get_errno());
    }
    _domain = domain;
    _type = type;
    _protocol = protocol;
}

void Socket::Setsockopt(int level, int option_name, const void *option_value, socklen_t option_len)
{
    int res;
    res = setsockopt(_socket_fd, level, option_name, option_value, option_len);
    if (res == -1)
    {
        throw std::runtime_error("Create socket Error:\n" +  get_errno());
    }
}

/* Destructor */
Socket::~Socket()
{
    close(_socket_fd);
}


// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <string.h>
// #include <stdexcept>
// #include <string.h>
// #include <iostream>

// class Socket
// {
// public:
//     Socket(int domain, int type, int protocol)
//     {
//         m_socket_fd = socket(domain, type, protocol);
//         if (m_socket_fd == -1)
//         {
//             throw std::runtime_error("Error creating socket");
//         }
//     }

//     Socket(std::string& ipaddr)
//     {

//     }


//     virtual ~Socket()
//     {
//         close(m_socket_fd);
//     }

//     void bind(int port)
//     {
//         sockaddr_in addr;
//         memset(&addr, 0, sizeof(addr));
//         addr.sin_family = AF_INET;
//         addr.sin_port = htons(port);
//         addr.sin_addr.s_addr = INADDR_ANY;

//         if (::bind(m_socket_fd, (sockaddr*)&addr, sizeof(addr)) == -1)
//         {
//             throw std::runtime_error("Error binding socket");
//         }
//     }

//     void listen()
//     {
//         if (::listen(m_socket_fd, SOMAXCONN) == -1)
//         {
//             throw std::runtime_error("Error listening on socket");
//         }
//     }

//     Socket accept()
//     {
//         sockaddr_in addr;
//         socklen_t addr_len = sizeof(addr);
//         int client_fd = ::accept(m_socket_fd, (sockaddr*)&addr, &addr_len);
//         if (client_fd == -1) {
//             throw std::runtime_error("Error accepting connection");
//         }

//         Socket client_socket;
//         client_socket.m_socket_fd = client_fd;
//         return client_socket;
//     }

//     void connect(std::string host, int port)
//     {
//         sockaddr_in addr;
//         memset(&addr, 0, sizeof(addr));
//         addr.sin_family = AF_INET;
//         addr.sin_port = htons(port);
//         inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

//         if (::connect(m_socket_fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
//             throw std::runtime_error("Error connecting to server");
//         }
//     }

//     ssize_t send(const std::string& data)
//     {
//         return ::send(m_socket_fd, data.c_str(), data.size(), 0);
//     }

//     ssize_t receive(std::string& data)
//     {
//         char buffer[1024];
//         ssize_t bytes_received = ::recv(m_socket_fd, buffer, sizeof(buffer), 0);
//         if (bytes_received > 0) {
//             data = std::string(buffer, bytes_received);
//         }
//         return bytes_received;
//     }

// private:
//     int m_socket_fd;
// };
