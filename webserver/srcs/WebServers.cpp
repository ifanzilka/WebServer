#ifndef IRCSERVER
# define IRCSERVER
#include <WebServers.hpp>
#include <HttpResponse.hpp>


#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <strings.h>



#include <iostream>
#include <string>
#include <sstream>
#include <vector>


struct HttpRequest
{
    std::string method;
    std::string path;
    std::string version;
    std::vector<std::string> headers;
    std::string body;
};

class HttpRequestParser
{
public:
    HttpRequest parse(std::string request) {
        HttpRequest http_request;

        // Split the request into lines
        std::vector<std::string> lines = ft_split(request, "\r\n");

        // Parse the first line
        std::vector<std::string> parts = ft_split(lines[0], " ");
        http_request.method = parts[0];
        http_request.path = parts[1];
        http_request.version = parts[2];

        // Parse the headers
        for (int i = 1; i < lines.size(); i++) {
            http_request.headers.push_back(lines[i]);
        }

        // Parse the body
        int body_index = request.find("\r\n\r\n");
        if (body_index != std::string::npos) {
            http_request.body = request.substr(body_index + 4);
        }

        return http_request;
    }

private:
};





/* Constructos */

WebServer::WebServer(int port)
{
    std::string tmp = "127.0.0.1";
            
    WebServer::InitServ(tmp, port);

}


WebServer::WebServer(std::string &ipaddr, int port)
{
    WebServer::InitServ(ipaddr, port);
}


WebServer::WebServer(const char *ipaddr, int port)
{
    std::string tmp = std::string(ipaddr);

    WebServer::InitServ(tmp, port);
}

/* Destructor */

WebServer::~WebServer()
{
    delete _MainServer;
}

void WebServer::InitServ( std::string ipaddr, int port)
{

    // _MainServer = new ServerSelect(ipaddr, port);
    //_MainServer = new ServerPoll(ipaddr, port);
    // _MainServer = new ServerKqueue(ipaddr, port);
    // _MainServer = new ServerEpoll(ipaddr, port);

    #if defined(SELECT)
        _MainServer = new ServerSelect(ipaddr, port);
        _MainServer->Logger(GREEN, std::string("Server Type Select"));
    #elif defined(POLL)
        _MainServer = new ServerPoll(ipaddr, port);
        _MainServer->Logger(GREEN, std::string("Server Type Poll"));
    #elif defined(EPOLL)
        _MainServer = new ServerEpoll(ipaddr, port);
        _MainServer->Logger(GREEN, std::string("Server Type Epoll"));
    #else
        _MainServer = new ServerKqueue(ipaddr, port);
        _MainServer->Logger(GREEN, std::string("Server Type Kqueue"));
    #endif

}


int WebServer::WaitEvent()
{
    int res_return;

    res_return = _MainServer->WaitEvent();
    return (res_return);
}

int WebServer::CheckConnect()
{
    int res_return;

    res_return = _MainServer->CheckConnect();
    return (res_return);
}

int WebServer::CheckDisconnect()
{
    int res_return;

    res_return = _MainServer->CheckDisconnect();
    return (res_return);
}

int WebServer::CheckAndRead()
{
    Client   *Client;
    int         fd;

    fd = _MainServer->CheckAndRead();
    if (fd > 1)
    {
        Client = _MainServer->GetClientFromFd(fd);
        
        if (Client != NULL)
        {
            //_MainServer->Logger(B_GRAY, "\n[" + Client->getName() + "] " + _MainServer->_msg);
        }
        _MainServer->Logger(B_GRAY, "Read in " + std::to_string(fd) + " fd");
    }
    return (fd);
}

int WebServer::SendInFd(int fd, std::string msg)
{
    Client  *client;
    int     res;

    client = _MainServer->GetClientFromFd(fd);
    if (client == NULL)
        return (0);

    std::cout << "\n";
    //_MainServer->Logger(PURPLE, std::string("[") + client->getNickName() + "] " + std::string("Attempt to send: "));
	_MainServer->Logger(B_BLUE, msg);
    std::cout << "\n";
    
    msg += "\r\n";
    res = _MainServer->SendInFd(fd, msg);
    //res = _MainServer->SendInFd(fd, "\n\r");
    return (res);
}

int WebServer::SendInFd(int fd, char* msg, int size)
{
    Client  *client;
    int     res;

    client = _MainServer->GetClientFromFd(fd);
    if (client == NULL)
        return (0);

    std::cout << "Send char* \n";
    //_MainServer->Logger(PURPLE, std::string("[") + client->getNickName() + "] " + std::string("Attempt to send: "));
	//_MainServer->Logger(B_BLUE, msg);
    //std::cout << "\n";
    
    //msg += "\r\n";
    res = _MainServer->SendInFd(fd, msg, size);
    res = _MainServer->SendInFd(fd, "\n\r");
    return (res);
}



void WebServer::Start()
{
    int     events;
    int     connect;
    int     disconnect;
    int     reads_fd;
    
    events = this->WaitEvent();
    while (1)
    {
        _MainServer->_msg="";
        if (events == 0)
            events = this->WaitEvent();

        connect = this->CheckConnect();
        if (connect > 0)
        {
            events--;
            continue;
        }
            

        disconnect = this->CheckDisconnect();
        if (disconnect > 0)
        {
            events--;
            continue;
        }
            
        std::string body("<html>\
                <body>\
                <h1>Hello, World!</h1>\
                </body>\
                </html>");


        reads_fd = this->CheckAndRead();
        if (reads_fd > 1)
        {   
            //HttpResponse::SendHTTPResponseFile(reads_fd, 200, "image/jpeg", "resource/fera.jpeg");
            //HttpResponse::SendHTTPResponseFile(reads_fd, 200, "video/mp4", "resource/nyan.mp4");
            
            HttpResponse::SendHTTPResponse(reads_fd, 200, "text/html", START_PAGE);
            // std::string res = HttpResponse::MakeHTTPResponse(200, "text/html", strlen(START_PAGE));

            // _MainServer->SendInFd(reads_fd, res.c_str());
            // _MainServer->SendInFd(reads_fd, START_PAGE);


            std::cout << "msg:\n" << _MainServer->_msg << "\n";
            // std::vector<std::string> lines = ft_split(_MainServer->_msg, "\r\n");

            // for (int i = 0; i < lines.size(); i++)
            // {
            //     std::cout << "lines:" << lines[i] << "\n";
            // }

            HttpRequestParser parser;
            HttpRequest http_request = parser.parse(_MainServer->_msg);
            std::cout << "Method: " << http_request.method << std::endl;
            std::cout << "Path: " << http_request.path << std::endl;
            std::cout << "Version: " << http_request.version << std::endl;
            std::cout << "Headers: " << std::endl;
            for (int i = 0; i < http_request.headers.size(); i++) {
                std::cout << "  " << http_request.headers[i] << std::endl;
            }
            std::cout << "Body: " << http_request.body << std::endl;


            events--;
        }
        
    }
    
}

# endif