#include <unordered_map>
#include <stack>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

#include "helper.h"

using namespace std;

class blueAgent
{
    public:
    static blueAgent* m_pAgent;

    public:
    blueAgent* getInstance()
    {
        if( m_pAgent == nullptr )
        {
            m_pAgent = new blueAgent();
        }
        
        return m_pAgent;
    }

    
    ~blueAgent();

    void addDevice(int fd, devInfo dev);
    void removeDevice(int fd);

    void run();
    void stop();

    

    private:
    std::unordered_map<int, devInfo> devList;

    //unhandled device
    std::stack<int> devNew;

    //thread indicator
    bool m_bContinue;

    blueAgent();
    void onNotify();
    
};