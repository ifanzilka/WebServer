#ifndef IRCSERVER
# define IRCSERVER
#include <WebServers.hpp>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <strings.h>


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
    autoindex = true;
    location = "/Users/ifanzilka/Desktop/Servers/serv2/webserver";

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
            


        reads_fd = this->CheckAndRead();
        if (reads_fd > 1)
        {   

            std::cout << "Message: " << _MainServer->_msg << std::endl;
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
            
            if (http_request.method == "GET")
            {
                WebServer::PreproccGetRequest(parser, reads_fd);
            }
            else if (http_request.method == "POST")
            {
                std::cout << "Post request";
                
                // std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text\r\nContent-Length: 5\r\n\r\nHello";
                // send(reads_fd, response.c_str(), response.length(), 0);

                //const char *response = "HTTP/1.1 201 Created\r\nContent-Length: 5\r\n\r\nHello";
                //send(reads_fd, response, std::strlen(response), 0);

                // const char *response = "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nInvalid request";
                // send(reads_fd, response, std::strlen(response), 0);
                //std::cout << "YES SEND\n";

                
                
                //HttpResponse::SendHTTPResponse(reads_fd, 200, "text/html", (char *)PAGE_START);
            }


            std::cout << "msg:\n" << _MainServer->_msg << "\n";
            events--;
        }
        
    }
}


void WebServer::PreproccGetRequest(HttpRequestParser &parser, int reads_fd)
{
    HttpRequest http_request = parser.parse(_MainServer->_msg);

    if (http_request.path == "/")
    {
        
        if (WebServer::autoindex == true)
        {
            WebServer::SendDirRequest(reads_fd, WebServer::location  + std::string("/"));
            //WebServer::SendDirRequest(reads_fd, "./");
        }
        else
        {
            HttpResponse::SendHTTPResponse(reads_fd, 200, "text/html", (char *)PAGE_START);
        }
        
        //
    }
    else if (http_request.path == "/favicon.ico")
    {
        HttpResponse::SendHTTPResponseFile(reads_fd, 200, "image/x-icon", "resource/favicon.ico");
    }
    else
    {

        PathRequestPreprocc(reads_fd, http_request);
        //HttpResponse::SendHTTPResponse(reads_fd, 200, "text/html", (char *)PAGE_404);
    }

}

void WebServer::SendDirRequest(int fd, std::string dir)
{
    std::string html_path = "<html>\
    <head><title>Index of /www/</title></head>\
    <body>\
    <h1>Index of " + dir + "</h1><hr><pre><a href=\"..\">../</a>\n";

    std::cout << "html_path: "<< html_path << "\n";

    DIR *path_dir = opendir((const char *)dir.c_str());
    struct dirent *pDirent;
    struct stat     info;

    if (path_dir != NULL)
    {
        while ((pDirent = readdir(path_dir)) != NULL)
        {
            //printf ("[%s]\n", pDirent->d_name);
            // std::cout << "Filename: " << pDirent->d_name << "\n";
            // std::cout << "d_reclen: " << pDirent->d_reclen << "\n";
        
            if (std::string(pDirent->d_name) == ".." || std::string(pDirent->d_name) == ".")
                continue;

            if (stat( (dir + "/" + std::string(pDirent->d_name)).c_str(), &info) == 0)
            {
                if (S_ISDIR(info.st_mode))
                {
                    std::cout << GREEN << "PATH is dir!\n" << NORM;
                    html_path += "<a href=\"" + std::string(pDirent->d_name)  + "/\">" + std::string(pDirent->d_name)  + "/</a>\t\t\t"+"-\n";
                }
                else
                {
                    html_path += "<a href=\"" + std::string(pDirent->d_name)  + "\">" + std::string(pDirent->d_name)  + "</a>\t\t\t" +  std::to_string((int)info.st_size) +"\n";
                }
            }
    
        }

        closedir(path_dir);
    }

    html_path += "</pre><hr></body></html>";
    HttpResponse::SendHTTPResponse(fd, 200, "text/html", (char*)html_path.c_str());

}

void WebServer::PathRequestPreprocc(int fd, HttpRequest &request)
{

    std::string path = WebServer::location +  request.path;
    char *str_path = (char *)path.c_str();


    //std::cout << GREEN << str_path << NORM <<std::endl; // path in filesystem
    struct stat st;
    if (stat(str_path, &st) == 0)
    {
        if (S_ISREG(st.st_mode))
        {
            //file send
            HttpResponse::SendHTTPResponseFile(fd, 200, "multipart/form-data", path);

        }
        else if (S_ISDIR(st.st_mode))
        {
            //path send
            SendDirRequest(fd, str_path);
        }
        else
        {
            std::cout << PURPLE << "Что-то другое существует по данному пути\n" << NORM;
        }
    }
    else
    {
        std::cout << RED << "File and dir is not open!\n" << NORM;
    }

    

    // DIR *path_dir = opendir((const char *)str_path);
    // if (path_dir != NULL)
    // {
    //     std::cout << GREEN << "PATH is open!\n" << NORM;
    //     //
    // }
    // else
    // {
    //     std::cout << RED << "PATH is not open!\n" << NORM;
    //     HttpResponse::SendHTTPResponse(fd, 200, "text/html", (char *)PAGE_404);   
    // }

    // //check file
    // FILE *file = fopen((const char*)str_path, "r");
    // if (file != NULL)
    // {
    //     std::cout << GREEN << "File is open!\n" << NORM;
    //     //
    //     return;
    // }
    // else
    // {
    //     std::cout << RED << "File is not open!\n" << NORM;
    // }



    
}

# endif