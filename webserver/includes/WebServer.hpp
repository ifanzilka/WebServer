#include "Socket.hpp"
#include "SocketsApi.hpp"


#define SERVER_PROTOCOL 	AF_INET 	//IpV4
#define	SERVER_TYPE			SOCK_STREAM //TCP

class HttpServer
{
    public:

        /* Constructors */
        HttpServer();                                  /* http://localhost:4242 */
        HttpServer(int port);                          /* http://localhost:port */
        HttpServer(const char *ipaddr,  int port);     /* http://ipaddr:port */
        HttpServer(std::string &ipaddr, int port);     /* http://ipaddr:port */
        


        /* Destructor */
        virtual ~HttpServer();
    
        void    start();
        void    stop();

    

private:
    Socket      _socket;
    SocketsApi _socket_api;
};
