#include <unordered_map>
#include <stack>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "helper.h"

#pragma once

using namespace std;

#define BLUE_ERROR -1
#define BLUE_ECONN -2
#define BLUE_EREAD -3

typedef struct _DEVICE_HANDLE
{
    int blue_sock;
    int dev_discriptor;
    struct hci_conn_info_req* cr;
}DEVICE_HANDLE;

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

    //struct hci_conn_info_req *cr;
    //device discriptor
    //int m_dd; 
    
    std::unordered_map<int, devInfo> m_devList;

    //unhandled device
    std::stack<int> m_devNew;

    //thread indicator
    bool m_bContinue;

    CBlueAgent();
    
    void onNotify(int fd, DevState state);

    int InitBlueDev(int fd, DEVICE_HANDLE& dh);

    void DisposeHandle(DEVICE_HANDLE& dh);

    int GetRSSI(int& rssi, DEVICE_HANDLE& dh);
};