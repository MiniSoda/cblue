#pragma once
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <memory>
#include <thread>
#include <mutex> 
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>
#include <stack>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>


enum SignalState{
    NIL,
    WEAKSIG,
    WITHIN
};


enum errorState{
    EUNDEFINED,
    EHCI,
    EIOCTL,
    EBLUE,
    ENOTFOUND
};

struct DeviceDescriptor
{
    DeviceDescriptor()
    {
        fd = 0; 
        bt_sock = 0;
        handle = 0;
    }
    //device descriptor
    int fd; 
    //bluetooth socket
    int bt_sock;
    struct hci_conn_info_req* handle;
};


class HciManager{
public:
    HciManager() = delete;
    HciManager( std::function<void (std::string&)> NilHandler,
            std::function<void (std::string&)> WeakHandler,
            std::function<void (std::string&)> WithinHandler,
            std::function<void (std::string&)> ErrorHandler 
        ) :
        m_onNilHandler(NilHandler),
        m_onWeakSigHandler(WeakHandler),
        m_onWithinHandler(WithinHandler),
        m_onErrorHandler(ErrorHandler)
    {
        m_threadContinue = true;
    };

    ~HciManager( )
    {
        for( auto item : m_DeviceDes )
        {
            if( item.second.bt_sock != 0 )
            {
                close(item.second.bt_sock);
                item.second.bt_sock = 0;
            }

            if( item.second.handle != 0 )
            {
                free(item.second.handle);
                item.second.handle = 0;
            }
        }

        if( m_hci )
        {
            hci_close_dev(m_hci);
            m_hci = 0;
        }
    };
    
    // threshold , interval
    bool Init(int threshold,int pollInterval);
    
    void StartService();
    void Stop();

    void addDevice(std::string dev);
    void removeDevice(std::string dev);

    void SetPollInterval(int pollInterval);

    std::vector<std::string> listConnections() const;

private:
    //endpoint address
    std::vector<std::string> m_devices;
    int m_signalThreshold;
    int m_pollInterval;
    bool m_threadContinue;
    
    //endpoint state
    std::map<std::string,SignalState> m_state;
    
    //hco device descriptor
    int m_hci;
    //endpoint device descriptor
    std::map<std::string, DeviceDescriptor> m_DeviceDes;

    //callback functions
    std::function<void (std::string&)> m_onNilHandler;
    std::function<void (std::string&)> m_onWeakSigHandler;
    std::function<void (std::string&)> m_onWithinHandler;
    std::function<void (std::string&)> m_onErrorHandler;

    void Clean(std::string address);
    bool Connect(std::string address, errorState& state);
    void readRSSI(const std::string& address, int& rssi, errorState& state);

    void EventHandler(std::string fd, const SignalState& state);
    void UpdateState(std::string fd, const SignalState& state)
    {
        auto item = m_state.find(fd);
        if( item != m_state.end() && item->second != state )
        {
            item->second = state;
            EventHandler( fd, state );
        }
    };

    
};