#include <unordered_map>
#include <stack>

#include "helper.h"

using namespace std;

#define BLUE_ERROR -1
#define BLUE_ECONN -2
#define BLUE_EREAD -3

class CBlueAgent
{
    public:

    static CBlueAgent& getInstance()
    {
        static CBlueAgent theSingleAgent;
        return theSingleAgent; 
    }

    
    ~CBlueAgent()
    {
        
    }

    void addDevice(int fd, unsigned char* pszAddr);
    void removeDevice(int fd);

    void run();
    void stop();

    

    private:

    std::unordered_map<int, devInfo> m_devList;

    //unhandled device
    std::stack<int> m_devNew;

    //thread indicator
    bool m_bContinue;

    CBlueAgent();
    
    void onNotify(int fd, DevState state);

    int GetRSSI(int fd, int& rssi);
};