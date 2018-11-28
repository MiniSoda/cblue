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
    
    CBlueAgent* m_pAgent;

    CBlueServer( );
};