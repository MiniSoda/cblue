#include <unordered_map>
#include <stack>

#include "helper.h"

using namespace std;

#define BLUE_ERROR -1
#define BLUE_ECONN -2
#define BLUE_EREAD -3

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
    std::unordered_map<int, devInfo> m_devList;

    //unhandled device
    std::stack<int> m_devNew;

    DevState m_state;

    //thread indicator
    bool m_bContinue;

    blueAgent();
    
    void onNotify(int id, DevState state);

    int GetRSSI(int fd, int& rssi);
};