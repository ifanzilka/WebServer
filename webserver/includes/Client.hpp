#ifndef FT_CLIENTIRC
# define FT_CLIENTIRC


class Client
{
	public:

		/* Constructor */
		Client(int fd_client, sockaddr_in 	addrinfo_client, std::string server_ipaddr)
		{
			_fd = fd_client;
			_addrinfo = addrinfo_client;
			_server_ipaddr = server_ipaddr;
            GetNameInfo((sockaddr *)&addrinfo_client);
                    
		};

		int getFd() const
		{ 
			return (_fd);
		}

        void GetNameInfo(const sockaddr * clientaddr)
        {
            char 				hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
            //socklen_t           addrlen;         /* input */

            if (getnameinfo(clientaddr, sizeof (struct sockaddr), hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV))
            {
               printf("Error!\n");
            }
            //printf("host=%s, serv=%s\n", hbuf, sbuf);
            std::string			hosts(hbuf);
            _host  = hosts;
        }



		struct sockaddr_in getAddrInfo() const
		{
			return (_addrinfo);
		}
	protected:
		struct sockaddr_in 	_addrinfo;
		int					_fd;
		std::string			_server_ipaddr;
        std::string         _host;


};

#endif
