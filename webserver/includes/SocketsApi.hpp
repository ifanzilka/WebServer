#ifndef SOCKETS_API
# define SOCKETS_API

#include "Socket.hpp"

class SocketsApi
{
    /* Constructor */
    SocketsApi(Socket socket_cls);    

    /* Говорю что можно переопределить */
    virtual	void		Init(std::string& ipaddr, int port);
    virtual	void		InitVariable();
    virtual int 		Create_socket();
    virtual int 		Binded();
    virtual int 		Listen();
    virtual	int 		Accept();
    virtual	int 		ReadInFd(int fd);
    virtual	int 		SendInFd(int fd, std::string msg);
    virtual	int 		SendInFd(int fd, const char *msg);
    virtual	int			SendInFd(int fd, const char *msg, size_t size);

    private:
        Socket  _socket_cls;
};


#endif