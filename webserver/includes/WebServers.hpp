#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

#include "WebServHeaders.hpp"

#include "ServerApi.hpp"
#include <ServerPoll.hpp>
#include <ServerSelect.hpp>
#include <ServerKqueue.hpp>
#include <ServerEpoll.hpp>
#include <utils.hpp>

#include <HtttpRequests.hpp>
#include <HttpResponse.hpp>

#include <stdio.h>  //fopen
#include <dirent.h> //opendir
#include <sys/stat.h>


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
        int     SendInFd(int fd, char* msg, int size);
        void    Start();

        bool        autoindex;
        std::string location;

        
    protected:

        /* Главный сервер для работы с сокетами */
        AbstractServerApi           *_MainServer;
        std::map<int, std::string>  _ChunkedBuffer;
        std::map<int, std::string>  _ChunkedBufferPost;
        std::map<int, std::string>  _ChunkedBufferPut;

    private:

        void InitServ(std::string ipaddr, int port);

        void PathRequestPreprocc(int fd, HttpRequest &request);
        void SendDirRequest(int fd, std::string dir);
        void PreproccGetRequest(HttpRequestParser &parser, int reads_fd);

};

#endif
