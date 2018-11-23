#include "BlueServer.h"

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
    if (sockfd < 0) 
    {
        error("ERROR opening socket");
        close(m_Serverfd);
        return bInit;
    }
        
    int enable = 1;
    if (setsockopt(m_Serverfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        error("setsockopt(SO_REUSEADDR) failed");
        close(m_Serverfd);
        return bInit;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    if (bind(m_Serverfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        error("ERROR on binding");
        m_Serverfd
        return bInit;
    }
            
    listen(m_Serverfd,5);
    bInit = true;

    m_pAgent = blueAgent::getInstance();

    return bInit;
}

bool Run()
{
    struct sockaddr_in cli_addr;
    clilen = sizeof(cli_addr);
    stack<thread*> threadlist;

    while(true)
    { 
        thread* t;
        int clientfd = accept(m_Serverfd, 
            (struct sockaddr *) &cli_addr, &clilen);
        if (clientfd < 0) 
        {
            error("ERROR on accept");
        }
        else
        {
            //do nothing
        }
            
        t = new thread( [&](clientfd) {

            while(true)
            {
                int n;
                COMM_PAKT buffer = { 0 };
                    
                n = read(clientfd,buffer,sizeof(buffer));
                if (n < 0) 
                {
                    error("ERROR reading from socket");
                }
                
                if( buffer.HEADER[0] != 0xFF && buffer.HEADER[1] != 0xE0)
                {
                    //wrong header
                    error("ERROR on packet");
                    close(clientfd);
                }
                else
                {
                    //do nothing
                }

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

                        m_pAgent->addDevice(clientfd, buffer.payload.LISTEN_ADDR);
                        n = write(sock, (void*)reply, sizeof(COMM_PAKT));
                        if (n < 0)
                        {
                            error("ERROR writing to socket");
                        }
                        break; 
                    }                     
                }
            }
                
        });
    }

    while(!threadlist.empty())
    {
        thread* t;
        t = threadlist.top();
        threadlist.pop();
        if(t->joinable())
        {
            t->join();  
        }
        delete t;
    }
}