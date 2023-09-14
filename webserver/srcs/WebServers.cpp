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


void sendResponse(int clientSocket, const std::string& responseData) {
    std::string response = "HTTP/1.1 201 Created\r\n";
    response += "Content-Type: text/plain\r\n";
    response += "Content-Length: " + std::to_string(responseData.length()) + "\r\n";
    response += "\r\n";
    response += responseData;

    // Отправка ответа на клиентский сокет
    ssize_t bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
    if (bytesSent == -1) {
        std::cerr << "Failed to send response" << std::endl;
    } else {
        std::cout << "Response sent (" << bytesSent << " bytes)" << std::endl;
    }

    // Закрытие клиентского сокета
    //close(clientSocket);
}

long long	ft_gettime(void)
{
	struct timeval	tp;
	long long		time;

	gettimeofday(&tp, NULL);
	time = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	return (time);
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
            
            //_ChunkedBuffer.erase(disconnect);
            continue;
        }
        

        reads_fd = this->CheckAndRead();
        if (reads_fd > 1)
        {


            if (_ChunkedBufferAll.find(reads_fd) == _ChunkedBufferAll.end())
            {
                _ChunkedBufferAll[reads_fd] = _MainServer->_msg;
            }
            else
            {
                _ChunkedBufferAll[reads_fd] += _MainServer->_msg;
            }
            if (_ChunkedBufferAll[reads_fd].length() > 4 && _ChunkedBufferAll[reads_fd].substr(_ChunkedBufferAll[reads_fd].length() - 4, _ChunkedBufferAll[reads_fd].length()) == std::string("\r\n\r\n"))
            {
                std::cout << "All Msg Parsing...\n";
                std::cout << _ChunkedBufferAll[reads_fd];

                HttpRequestParser parser;
                HttpRequest http_request = parser.parse(_ChunkedBufferAll[reads_fd]);

                _MainServer->_msg = _ChunkedBufferAll[reads_fd];
                std::cout << "Method: " << http_request.method << std::endl;
                std::cout << "Path: " << http_request.path << std::endl;
                std::cout << "Version: " << http_request.version << std::endl;
                std::cout << "Headers: " << std::endl;


                bool transfer_encoding = false;
                for (int i = 0; i < http_request.headers.size(); i++)
                {
                    std::cout << "Head: " << http_request.headers[i] << std::endl;
                   
                    size_t		nFind;
                    nFind = http_request.headers[i].find("Transfer-Encoding: chunked");
                    if (nFind != std::string::npos)
                        transfer_encoding = true;

                }
                std::cout << "transfer_encoding " << transfer_encoding <<"\n";
                std::cout << "Body: " << http_request.body << std::endl;


                if (http_request.method == "GET") //получение чего либо
                {
                    WebServer::PreproccGetRequest(parser, reads_fd);
                    _ChunkedBufferAll[reads_fd] = "";
                    _ChunkedBufferAll.erase(reads_fd);

                }
                else if (http_request.method == "POST") //отправки данных на сервер
                {

                    
                    if (transfer_encoding)
                    {
                        if ( _ChunkedBufferAll[reads_fd].length() > 5 &&  _ChunkedBufferAll[reads_fd].substr(_ChunkedBufferAll[reads_fd].length() - 5, _ChunkedBufferAll[reads_fd].length()) == std::string("0\r\n\r\n"))
                        {
                            
                            std::cout << "Chunked end parsing2\n";
                            
                            if (http_request.path == "/")
                            {
                                std::cout << 1 << "\n";
                                const char *response = "HTTP/1.1 405  Method Not Allowed\r\nContent-Length: 12\r\n\r\nInvalid request";
                                send_bytes_fd(reads_fd, response, std::strlen(response));
                            }
                            else
                            {
                                std::cout << 2 << "\n";
                                const char* response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/plain\r\n"
                               "Content-Length: 7\r\n"
                               "\r\n"
                               "Success";

                                send(reads_fd, response, strlen(response), 0);

                            }
                             _ChunkedBufferAll.erase(reads_fd);
                        }
                        else
                        {
                            std::cout << "Wait chunked";
                        }

                    }
    


                }
                else if (http_request.method == "HEAD")//запрашивает заголовки
                {

                    const char *response = "HTTP/1.1 405  Method Not Allowed\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
                    send(reads_fd, response, std::strlen(response), 0);
                    // _ChunkedBufferAll[reads_fd] = "";
                    _ChunkedBufferAll.erase(reads_fd);
                
                }
                else if (http_request.method == "PUT")//запрашивает заголовки
                {
                        
                    if ( _ChunkedBufferAll[reads_fd].length() > 5 &&  _ChunkedBufferAll[reads_fd].substr(_ChunkedBufferAll[reads_fd].length() - 5, _ChunkedBufferAll[reads_fd].length()) == std::string("0\r\n\r\n"))
                    {
                        std::cout << "Chunked end parsing1n";

                        std::string responseData = "Resource created";
                        sendResponse(reads_fd, responseData);
                        _ChunkedBufferAll.erase(reads_fd);
                    }
                    else
                    {
                        std::cout << "Wait chunked";
                    }
                
                }
            }




            }
            //_ChunkedBuffer.erase(reads_fd);
            events--;
        
        
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
    else if (http_request.path == "/directory/Yeah")
    {
        HttpResponse::SendHTTPResponse(reads_fd, 404, "", "");
    }
    else if (http_request.path == "/favicon.ico")
    {
        HttpResponse::SendHTTPResponseFile(reads_fd, 200, "image/x-icon", "resource/favicon.ico");
    }
    else
    {
        std::cout << "2\n" << "!" << http_request.path <<"!\n";
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
    //std::cout << "html: " << html_path << "\n";
    std::cout << "Send HttpResponse \n";
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
            std::cout << PURPLE << "ОТправляю содержмое папки\n" << NORM;
            SendDirRequest(fd, str_path);
        }
        else
        {
            std::cout << PURPLE << "Что-то другое существует по данному пути\n" << NORM;
        }
    }
    else
    {
        //File Not Found
        HttpResponse::SendHTTPResponse(fd, 404, "", "");
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