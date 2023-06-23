#ifndef WEBSERVER
# define WEBSERVER

#include "WebServer.hpp"

/* Constructors */
HttpServer::HttpServer()
{
    std::string tmp = "127.0.0.1";
    int         port = 4242;
    
    HttpServer(tmp, port);
}

HttpServer::HttpServer(int port)
{
    std::string tmp = "127.0.0.1";
    HttpServer(tmp, port);
}

HttpServer::HttpServer(const char *ipaddr,  int port)
{
    std::string tmp = std::string(ipaddr);
    HttpServer(tmp, port);
}

HttpServer::HttpServer(std::string &ipaddr, int port)
{
    /* Other Realization */

    _socket = Socket(SERVER_PROTOCOL, SERVER_TYPE , 0);
    //_socket_api = SocketApi(_socket);
    #if defined(SELECT)
        
    #elif defined(POLL)

    #elif defined(EPOLL)

    #else /* Kqueue  */

    #endif
}

HttpServer::~HttpServer()
{

}


#endif
