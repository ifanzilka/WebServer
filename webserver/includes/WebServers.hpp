#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

#include "ServerApi.hpp"
#include <ServerPoll.hpp>
#include <ServerSelect.hpp>
#include <ServerKqueue.hpp>
#include <ServerEpoll.hpp>

// #define SELECT  0
// #define POLL    1
// #define EPOLL   2
// #define KQUEUE  3


// class Channel;

class WebServer
{
    public:

        /* Constructors */
        WebServer(int port);

		WebServer(std::string &ipaddr, int port);

		WebServer(const char *ipaddr, int port);

        /* Destructor */
        ~WebServer();

        /* Function Main Server */

        int     WaitEvent();
        int     CheckConnect();
        int     CheckDisconnect();
        int     CheckAndRead();
        int     SendInFd(int fd, std::string msg);
        void    Start();

        
    protected:

        /* Главный сервер для работы с сокетами */
        AbstractServerApi           *_MainServer;

    private:

        void InitServ(std::string ipaddr, int port);

};

#endif
