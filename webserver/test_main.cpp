#include <iostream>
#include <WebServers.hpp>
#include <signal.h>




int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);//Если произошел дисконект не падать
    (void)argc;
    (void)argv;    
    
    if (argc > 1)
	{
        WebServer serv("127.0.0.1", atoi(argv[1]));
		//ServerSelect  serv("127.0.0.1", atoi(argv[1]));
        //ServerPoll    serv("127.0.0.1", atoi(argv[1]));
        //ServerKqueue  serv("127.0.0.1", atoi(argv[1]));
        //ServerEpoll serv("127.0.0.1", atoi(argv[1]));
        
        // while (1)
        // {
        //     serv.Start();
        // }

        // int events;
        // while (1)
        // {
        //     events = serv.WaitEvent();
        //     int connect = serv.CheckConnect();
        //     if (connect > 0)
        //         continue;
            
        //     connect = serv.CheckDisconnect();
        //     if (connect > 0)

        //         continue;

        //     serv.CheckAndRead();
        // }

        serv.Start();
    }
	else
	{
		std::cout << RED"Use: WebServer <port> \n" << F_NONE;
	}

    return (42);
}