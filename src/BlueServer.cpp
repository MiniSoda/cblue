#include <csignal>
#include <poll.h>

#include "BlueServer.h"
#include "agent.h"


CBlueServer::CBlueServer( )
{

}

CBlueServer::~CBlueServer( )
{
    
}

bool CBlueServer::InitServer()
{
    struct sockaddr_in serv_addr;
    bool bInit = false;
    m_Serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_Serverfd < 0) 
    {
        perror("ERROR opening socket");
        close(m_Serverfd);
        return bInit;
    }
        
    int enable = 1;
    if (setsockopt(m_Serverfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        close(m_Serverfd);
        return bInit;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    if (bind(m_Serverfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        perror("ERROR on binding");
        close(m_Serverfd);
        return bInit;
    }
            
    listen(m_Serverfd,5);
    bInit = true;

    return bInit;
}

bool CBlueServer::Run()
{
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
   
    m_sockets.clear();

    thread bt(
        [&](){
            CBlueAgent::getInstance().run();
        }
    );

    std::thread t(
        [this]{
            polling();
        }
    );

    if(t.joinable())
    {
        t.detach();
    }

    while(true)
    { 
        //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        int clientfd = accept(m_Serverfd, 
            (struct sockaddr *) &cli_addr, &clilen);
        
        if (clientfd < 0) 
        {
            perror("ERROR on accept");
        }
        else
        {
            m_sockets.push_back(clientfd);
            m_Newfd = true;
        }
    }

    
    
    CBlueAgent::getInstance().stop();

    if(bt.joinable())
    {
        bt.join();
    }
}

void CBlueServer::polling( )
{
    bool wait_for_client = true;
    
    while(wait_for_client)
    {
        int timeout = 500; 
        int fd_size = m_sockets.size(); 

        if( fd_size == 0 )
        {
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }

        struct pollfd pollfds[fd_size];  

        //polling sockfd
        int index = 0; 
        for( auto fd : m_sockets )
        {
            pollfds[index].fd = fd;
            pollfds[index].events = POLLIN|POLLRDHUP;    
            index++;

            if( index>0 ) 
                m_Newfd = false;
        }
                  
        //set event id 
        while( !m_Newfd && (index != 0) )
        { 
            int poll_val = poll( pollfds, fd_size, timeout);
            if( poll_val >0 )
            {
                for( int i=0; i<fd_size;i++)
                {
                    int clientfd = pollfds[i].fd;

                    if ( (pollfds[i].revents & POLLIN) != 0) 
                    {
                        //recv
                        int n;
                        COMM_PAKT buffer = { 0 };
                        size_t len = sizeof(COMM_PAKT);
                        
                        n = read(clientfd, (void*)&buffer, len);
                        if (n < 0) 
                        {
                            perror("ERROR reading from socket");
                        }
                        
                        if( buffer.HEADER[0] != 0xFF && buffer.HEADER[1] != 0xE0)
                        {
                            //wrong header
                            perror("ERROR on packet");
                            shutdown(clientfd, SHUT_RDWR);
                            close(clientfd);
                        }
                        else
                        {
                            //do nothing
                            switch(buffer.CMD)
                            {
                                case CLI_REGISTER:
                                case CLI_INQUIRE:
                                {
                                    COMM_PAKT reply = {0};

                                    reply.HEADER[0]= 0xFF;
                                    reply.HEADER[1]= 0xE0;
                                    reply.HEADER[2]= 0xFF;
                                    reply.HEADER[3]= 0xE0;

                                    reply.CMD = CMDType::ACK;

                                    CBlueAgent::getInstance().addDevice(clientfd, buffer.payload.LISTEN_ADDR);
                                    n = write(clientfd, (void*)&reply, sizeof(COMM_PAKT));
                                    if (n < 0)
                                    {
                                        perror("ERROR writing to socket");
                                        wait_for_client = false;
                                    
                                        CBlueAgent::getInstance().removeDevice(clientfd);
                                        shutdown(clientfd, SHUT_RDWR);
                                        close(clientfd);
                                    }
                                    break; 
                                }    
                                case CLI_EXIT:
                                {
                                    COMM_PAKT reply = {0};

                                    reply.HEADER[0]= 0xFF;
                                    reply.HEADER[1]= 0xE0;
                                    reply.HEADER[2]= 0xFF;
                                    reply.HEADER[3]= 0xE0;

                                    reply.CMD = CMDType::ACK;
                                    n = write(clientfd, (void*)&reply, sizeof(COMM_PAKT));

                                    wait_for_client = false;
                                    
                                    CBlueAgent::getInstance().removeDevice(clientfd);
                                    shutdown(clientfd, SHUT_RDWR);
                                    close(clientfd);
                                    break;
                                }                 
                            }
                              
                        }

                           
                    }
                    else if( (pollfds[i].revents & (POLLRDHUP|POLLNVAL|POLLERR) ) != 0 )
                    {
                        //connection down
                        CBlueAgent::getInstance().removeDevice(clientfd);
                        //shutdown(clientfd, SHUT_RDWR);
                        close(clientfd);
                        
                        auto iter = m_sockets.begin()+i;
                        if( iter != m_sockets.end() )
                        {
                            m_sockets.erase(iter);
                        }
                        index--;
                    }
                }
            }
            else
            {
                //error in poll
            }
        }   
    }      
}