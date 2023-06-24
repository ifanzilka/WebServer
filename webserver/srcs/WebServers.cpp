#ifndef IRCSERVER
# define IRCSERVER
#include <WebServers.hpp>
#include <HttpResponse.hpp>


#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <strings.h>

// std::string read_file(char *filename)
// {
//     char 		    buffer[BUFFER_SIZE];

//     std::string     full_buf = "";
// 	bzero(buffer, BUFFER_SIZE);

    
//     int fd  = open(filename, O_RDONLY);
// 	int ret = read(fd, buffer, BUFFER_SIZE - 1);
	
//     return buffer;
//     if (ret == 0)
// 	{
// 		return (NULL);
// 	}
// 	else
// 	{
//         full_buf += buffer;

//         /* Read */
// 		while (ret > 0)
// 		{
//             std::cout << ret << std::endl;
// 			ret = read(fd, buffer, BUFFER_SIZE - 1);
// 			if (ret == -1)
// 				break;

// 			buffer[ret] = 0;
// 			full_buf += buffer;
//             bzero(buffer, BUFFER_SIZE);
// 		}
//         //std::cout << ret << std::endl;
//         //std::cout << full_buf.length() << std::endl;
//     }
//     close(fd);
//     return (full_buf);
// }
//void HttpResponse::send_file(const std::string& file_path, const std::string& content_type) {
//     // Open the file in binary mode
//     std::ifstream file(file_path, std::ios::binary);
//     if (!file.is_open()) {
//         // If the file cannot be opened, throw an exception
//         throw std::runtime_error("Failed to open file: " + file_path);
//     }

//     // Read the contents of the file into a string
//     std::ostringstream contents;
//     contents << file.rdbuf();

//     // Send the contents as the response body with the appropriate content type
//     set_content_type(content_type);
//     set_body(contents.str());
// }



#include <vector>
#include <fstream>

static std::vector<char> ReadAllBytes(char const* filename)
{
    std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    std::vector<char>  result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], pos);

    return result;
}



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

            
            //std::string res_file = read_file("resource/linux_mini.png");
            //std::cout << res_file;
            std::vector<char> res_file = ReadAllBytes("resource/linux_mini.png");
            std::cout << res_file.size() << "\n";
            
            std::ifstream file("resource/linux.png", std::ios::binary);
            std::ostringstream contents;
        contents << file.rdbuf();

        // Send the contents as the response body with the appropriate content type
            // set_content_type(content_type);
            // set_body();
            this->SendInFd(reads_fd, HttpResponse::MakeHTTPRespone(200, "image/png", contents.str()));
            //this->SendInFd(reads_fd, contents.str(), res_file.size());
            events--;
        }
        
    }
    
}

# endif