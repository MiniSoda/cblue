#include <string>

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

    void InitServer();
    bool Run();

    private:
    short m_nPort;
    std::string m_Address;

    enum SERVER_STATE{
        //waiting for client
        WAITING,
        //tray client connected
        TC_CONNECTED,
        //Credential Client connected
        CL_CONNECTED
    };

    CBlueServer( );
    
};