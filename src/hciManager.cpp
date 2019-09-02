#include <algorithm>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <utility>

#include "hciManager.h"


bool HciManager::Init(int threshold,int pollInterval)
{
    m_signalThreshold = threshold;
    m_pollInterval = pollInterval;

    int dev_id =0;
    int result = 0;  
    bool state = true;
      
    //use default hci
    m_hci = hci_open_dev(dev_id);

    if (m_hci < 0)
    {
        perror("Could not open HCI device");
        state = false;
    }

    return state;
}


void HciManager::StartService()
{
    std::vector<std::thread> threads;

    std::function<void(const std::string& address)> thread_handler =
        [&](const std::string& fd)
        {
            bool connected = false;
            
            SignalState state = SignalState::NIL;
            errorState error = errorState::EUNDEFINED;

            while( m_threadContinue )
            {
                bool bStateChange = false; 

                if( m_devices.size() == 0 )
                {
                    break;
                }
                else
                {

                }

                if(!connected)
                {
                    connected = Connect(fd,error);
                }
               
                if( connected )
                {
                    int rssi = -255;
                    errorState readState = errorState::EUNDEFINED;
                    readRSSI(fd, rssi, readState);

                    if(readState == errorState::EBLUE) 
                    { 
                        state = SignalState::NIL;
                        UpdateState(fd, state);
                        
                        Clean( fd );
                        connected = false;
                    }
                    else if ( rssi < m_signalThreshold ) {
                        state = SignalState::WEAKSIG;
                        UpdateState(fd, state);
                    }
                    else
                    {
                        state = SignalState::WITHIN;
                        UpdateState(fd, state);
                    }  
                }else
                {
                    if( error == errorState::ENOTFOUND )
                    {
                        Clean(fd);
                        std::this_thread::sleep_for(std::chrono::seconds(m_pollInterval));
                        continue;
                    }
                    else if ( error == errorState::EHCI ) {
                        continue;
                    }
                    else if ( error == errorState::EIOCTL ) {
                        Clean(fd);
                        continue;
                    }
                }
                                
                std::this_thread::sleep_for(std::chrono::seconds(m_pollInterval));
            }
        };
       
        for( auto endpoint : m_devices )
        {
            threads.push_back( std::thread(thread_handler,endpoint) );
        }

        for( auto& t : threads )
        {
            if( t.joinable())
            {
                t.join();
            }
        }
}


bool HciManager::Connect(std::string address, errorState& state)
{
    bool result = false;
    int status = 0;
    
    struct sockaddr_l2 addr = { 0 };
    auto deviceDes = m_DeviceDes.find(address);
    
    if( deviceDes != m_DeviceDes.end() )
    {
        auto& endpoint = deviceDes->second;
        
        if( endpoint.handle == 0)
        {
            endpoint.handle = (hci_conn_info_req *)malloc(sizeof(struct hci_conn_info_req) + sizeof(struct hci_conn_info));
            if ( endpoint.handle == NULL)
            {
                perror("malloc");
            }
            state = errorState::EHCI;
        }
        else 
        {
            if(endpoint.bt_sock == 0 )
                endpoint.bt_sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
            
            addr.l2_family = AF_BLUETOOTH;
            //service discovery port
            addr.l2_psm = htobs(0x1);
            str2ba( address.c_str(), &addr.l2_bdaddr );

            // connect to target device
            status = connect(endpoint.bt_sock, (struct sockaddr *)&addr, sizeof(addr));
            
            if(status!=0)
            {
                perror("Could not connect to dev");
                state = errorState::ENOTFOUND;
            }
            else
            {
                bacpy(&endpoint.handle->bdaddr, &addr.l2_bdaddr);
                endpoint.handle->type = ACL_LINK;

                if ( ioctl(m_hci, HCIGETCONNINFO, (unsigned long)endpoint.handle ) < 0 )
                {
                    perror("Could not get connection info");
                    state = errorState::EIOCTL;
                }
                else
                {
                    result = true;
                    state = errorState::EUNDEFINED;
                }
            }            
        }
    }
    else  
    {
        
    }  
    return result;
}


void HciManager::readRSSI(const std::string& address, int& rssi, errorState& state)
{
    int8_t read = 0;
    auto endpoint = m_DeviceDes.find( address );
    auto deviceDes = endpoint->second;

    if (hci_read_rssi( m_hci, htobs(deviceDes.handle->conn_info->handle), &read, 0) < 0)
    {
        perror("Could not read RSSI");
        state = errorState::EBLUE;
    }
    rssi = read;
}


void HciManager::Clean(std::string address)
{
    auto endpoint = m_DeviceDes.find( address );
    auto& deviceDes = endpoint->second;

    if( deviceDes.bt_sock != 0)
        close( deviceDes.bt_sock );
    if( deviceDes.handle != 0)
        free( deviceDes.handle );
    if( deviceDes.fd!= 0 )
        hci_close_dev( deviceDes.fd );
    
    deviceDes.bt_sock = 0;
    deviceDes.handle = 0;
    deviceDes.fd = 0;
}


void HciManager::Stop()
{
    m_threadContinue = false;
}


void HciManager::addDevice(std::string dev)
{
    m_devices.push_back(dev);
    m_state.emplace(std::pair<std::string, SignalState>(dev, SignalState::NIL));
    m_DeviceDes.emplace(std::pair<std::string, DeviceDescriptor>(dev,DeviceDescriptor()));
}


void HciManager::removeDevice(std::string dev)
{
    auto item = std::find(m_devices.begin(),m_devices.end(),dev);
    if( item != m_devices.end())
    {
        m_devices.erase(item);
    }
}


void HciManager::SetPollInterval(int pollInterval)
{
    m_pollInterval = pollInterval;
}


std::vector<std::string> HciManager::listConnections( ) const
{
    return m_devices;
}


void HciManager::EventHandler(std::string fd, const SignalState& state)
{
    switch(state)
    {
        case SignalState::NIL:
        {
            m_onNilHandler(fd);
            break;
        }
        case SignalState::WEAKSIG:
        {
            m_onWeakSigHandler(fd);
            break;
        }
        case SignalState::WITHIN:
        {
            m_onWithinHandler(fd);
            break;
        }
        default:
            m_onErrorHandler(fd);
    }
}