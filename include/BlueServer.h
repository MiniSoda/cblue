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
    const short PORT = 8168;
    

    CBlueServer( );
};