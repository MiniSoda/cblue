#include "agent.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <string>
#include <thread>
#include <memory>



class CBlueServer
{
public:
    static CBlueServer& getInstance()
    {
        static CBlueServer BlueServer;        
        return BlueServer;
    }
    
    ~CBlueServer( );

    bool InitServer();
    bool Run();

private:
    const short PORT = 8168;
    int m_Serverfd, pid;
    bool m_Newfd;
    deque<int> m_sockets;
  
    void polling();
    CBlueServer( );
};