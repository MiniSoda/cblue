#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <string>
#include <thread>

#include "agent.h"

class CBlueServer
{
    
    static CBlueServer* m_pServer;

public:
    CBlueServer* getInstance()
    {
        if( m_pServer == nullptr )
        {
            m_pServer = new CBlueServer();
        }
        
        return m_pServer;
    }

    ~CBlueServer( );

    bool InitServer();
    bool Run();

private:
    const short PORT = 8168;
    int m_Serverfd, pid;
    
    CBlueAgent* m_pAgent;

    CBlueServer( );
};