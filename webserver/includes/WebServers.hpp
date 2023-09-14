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

enum ServerDef 
{
	//	** Socket Type
	SERVER_SOCK = 1,
	CLIENT_SOCK,

	//	** Request 
	KEEP_ALIVE = 7,
	CLOSE,
	SET_CNX, //? connexion not set

	//	** Request stat
	STAT = 199,
	WELL = 200,
	TIMEOUT,	//?: Request TimeOut

	//	**
	CRF = 4,
	TIMELIMIT = 5000	//?: 5 sec
};

class	ClientSock
{
	public :
		std::string		_host;				//?:	server name came in request Host header.
		//std::vector<Server>	vecServ;		//?:	Vector of servers that may handle request came in this socket.
		//ServerConf		_serverResponding;	//?:	Configuration of the server responsible of this client request.
		
		bool			_InitialRead;		//?:	true -> still initial read available.
		bool			_chunkedBody;		//?:	true -> body is chunked.
		bool			_readiness;			//?:	true -> socket is in write mode , send response.
		int				_connexion;			//?:	CLOSE -> connexion with client will be closed.
		int				_reqStat;			//?:	TIMEOUT -> request is on timeout.
		bool			_endChunk;			//?:	true -> if read of chunk body is done.

		std::string		_tmp;				//?:	where request is sotred until headers are separated than body.
		std::string		_reqHeader;			//?:	- After separation -> request Headers.
		std::string		_reqBody;			//?:	- After separation -> request body.
		long long		byteToRead;			//?:	-1 -> request with no body
		long long		byteRead;			//?:	bytes already read by recv().
		size_t			_nFind;				//?:	- chunked case -> Where find stopped until next chunked body read.
		long long		_skipedByte;		//?:	- chunked case -> Number of bytes skipped.
		std::string		_bodyChunk;			//?:	- chunked case -> whole body without chunked encoding delimiter.
		long long		_content_lenght;	//?:	- chunked case -> content_lenght of processed chunked body.
		std::string		_request;			//?:	- request formed to be processed by request-response process.
		long long		_lastRead;			//?:	- TimeOut case -> the last time read from socket.


		bool			_done;				//?:	true -> response send completely.
		std::string		_response;			//?:	response ready to be sent to client.
		long long		byteToSend;			//?:	size of response to be sent.
		long long		byteSent;			//?:	bytes already sent by send().

    ClientSock()
    {
        	_InitialRead = true;
            _chunkedBody = false;
            _readiness = false;
            //_connexion = SET_CNX;
            //_reqStat = STAT;
            _endChunk = false;
            _done = false;

            byteToRead = 0;
            byteRead = 0;

            _nFind = 0;
            _skipedByte = 0;
            _content_lenght = 0;

            byteToSend = 0;
            byteSent = 0;

            _lastRead = 0;
    }

    void	separateHeadBody(std::string tmp)
    {
        size_t		nFind;
        
        nFind = tmp.find("\r\n\r\n");
        if (nFind != std::string::npos)
        {
            _reqHeader = tmp.substr(0, nFind + 4);
            if (nFind + 4 < tmp.length())
                _reqBody = tmp.substr(nFind + 4, tmp.length());
            std::cout << "tmp clear() \n";
            _tmp.clear();
        }
        else
        {
            _InitialRead = true;
            _readiness = false;
        }
    }


    void	transferEncoding(void)
    {
        size_t		nFind;
        std::string	tmp;
        std::stringstream ss;
        
        nFind = _reqHeader.find("Transfer-Encoding: chunked");
        if (nFind != std::string::npos)
            _chunkedBody = true;
        else
        {
            _chunkedBody = false;
            nFind = _reqHeader.find("Content-Length: ");
            if (nFind != std::string::npos)
            {
                tmp = _reqHeader.substr(nFind + 16, _reqHeader.length() - (nFind + 16));
                ss << tmp;
                ss >> byteToRead;
                _content_lenght = byteToRead;
                byteToRead += _reqHeader.length();
            }
            else if (nFind == std::string::npos && _InitialRead == false)
                byteToRead = -1;
        }
    }

    std::string	findHeaderValue(std::string header)
    {
        size_t		nFind_1;
        size_t		nFind_2;
        std::string	findWord;
        std::string	word;

        nFind_1 = _reqHeader.find(header);
        if (nFind_1 != std::string::npos)
        {
            findWord = _reqHeader.substr(nFind_1 + header.length(), _reqHeader.length());
            nFind_2 = findWord.find("\r\n");
            if (nFind_2 != std::string::npos)
                word = _reqHeader.substr(nFind_1 + header.length(), nFind_2);
        }
        return word;
    }

    void	sockConnection(void)
    {
        std::string	conn;

        conn = this->findHeaderValue("Connection: ");
        if (!conn.compare("keep-alive"))
            _connexion = KEEP_ALIVE;
        else if	(!conn.compare("close"))
            _connexion = CLOSE;
        else
            _connexion = KEEP_ALIVE;
    }

    void	readChunkBody(void)
    {
        std::string			hexStr;
        long long			byte = 0;
        std::string			tmp;

        _nFind = _reqBody.find("\r\n", _skipedByte);
        while (_nFind != std::string::npos)
        {
            //?:	getting bytes to read
            hexStr = _reqBody.substr(_skipedByte, _nFind - _skipedByte);
            byte = std::strtol(hexStr.c_str(), NULL, 16);

            if (byte == 0)
            {
                _endChunk = true;
                break;
            }

            //?:	substring the body and check lenght == byte in next step.
            tmp = _reqBody.substr(_nFind + 2, byte);


            //?:	Saving Skipped bytes if bytes given are read well.
            if ((long long)tmp.length() == byte)
            {
                _skipedByte += hexStr.length() + byte + CRF;

                //?:	appending to body chunked to save body unchunked and content length.
                _bodyChunk.append(tmp);
                _content_lenght += byte;

                //?:	finding next chunked encoding delimiter.
                _nFind = _reqBody.find("\r\n", _skipedByte);
            }
            else
                break;
        }
}

};



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

        std::map<int, ClientSock>   clientMap;

        ///
        std::map<int, std::string>  _ChunkedBufferAll;
        std::map<int, std::string>  _ChunkedBuffer;
        std::map<int, std::string>  _ChunkedBufferPost;
        std::map<int, std::string>  _ChunkedBufferPut;
        std::map<int, std::string>  _HttpRequestInfo;

    private:

        void InitServ(std::string ipaddr, int port);

        void PathRequestPreprocc(int fd, HttpRequest &request);
        void SendDirRequest(int fd, std::string dir);
        void PreproccGetRequest(HttpRequestParser &parser, int reads_fd);

};

#endif
