#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <thread>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

#include "agent.h"

CBlueAgent::CBlueAgent()
{
    m_bContinue = true;
}

void CBlueAgent::addDevice(int fd, unsigned char* pszAddr)
{
    if (fd <= 0)
    {
        //error handling
    }
    else
    {
        devInfo dev;
        dev.fd = fd;
        memcpy(dev.LISTEN_ADDR, pszAddr, 30);
        dev.state = DevState::UNKNOWN;

        m_devNew.push(fd);
        m_devList.insert({fd,dev});
    }
}

void CBlueAgent::removeDevice(int fd)
{
    if (fd <= 0)
    {
        //error handling
    }
    else
    {
        m_devList.erase(fd);
    }
}

void CBlueAgent::run()
{
    stack<thread*> threadlist;
    while(m_bContinue)
    {      
        while( m_devNew.size()!= 0 )
        {
            //add dev to listen queue
            int fd = m_devNew.top();
            //bluetooth address of the device
            if(m_devList.end() != m_devList.find(fd))
            {
                thread* t = new thread([&]()
                {
                    int nRet = 0;
                    DevState state = DevState::UNKNOWN;

                    while(nRet != BLUE_ERROR)
                    {
                        bool bStateChange = false;

                        int rssi = -255;
                        nRet = GetRSSI(fd,rssi);

                        if( (nRet == BLUE_ECONN) || (rssi < -10) )
                        {
                            if(state != DevState::OUTOFRANGE)
                            {
                                state = DevState::OUTOFRANGE;
                                bStateChange = true;
                                onNotify( fd, state);
                            }
                            else
                            {
                                bStateChange = false;
                            }
                        }
                        else
                        {
                            if( (nRet == 0) && (state != DevState::WITHIN) )
                            {
                                state = DevState::WITHIN;
                                bStateChange = true;
                                onNotify( fd, state);
                            } 
                            else
                            {
                                bStateChange = false;
                            }
                        }

                        if( m_devList.end() == m_devList.find(fd))
                        {
                            break;
                        }
                        else
                        {

                        }

                        this_thread::sleep_for(chrono::seconds(2));
                    }
                });

                threadlist.push(t);
            }
            //remove
            m_devNew.pop();
        }

        if( m_devList.size() == 0)
        {
            std::this_thread::sleep_for( chrono::seconds(1) );
        }
    }

    while( !threadlist.empty())
    {
        thread* t;
        t = threadlist.top();
        threadlist.pop();
        if(t->joinable())
        {
            t->join();  
        }
        delete t;
    }
}

int CBlueAgent::GetRSSI(int fd, int& rssi)
{
    struct hci_conn_info_req *cr;
    
    int dd;
    int dev_id =0;
    int result = 0;  
    bool error = false;

    
    int8_t readRSSI = 0;

    struct sockaddr_l2 addr = { 0 };
    int sock, status;
    

    auto devMAC = m_devList.at(fd).LISTEN_ADDR;

    dd = hci_open_dev(dev_id);
    if (dd < 0)
    {
        perror("Could not open HCI device");
        result = BLUE_ERROR;
        goto CLEAR;
    }

    if ((cr = (hci_conn_info_req *)malloc(sizeof(struct hci_conn_info_req) + sizeof(struct hci_conn_info))) == NULL)
    {
        perror("malloc");
        result = BLUE_ERROR;
        goto CLEAR;
    }

    sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    addr.l2_family = AF_BLUETOOTH;
    //service discovery port
    addr.l2_psm = htobs(0x1);
    str2ba( devMAC, &addr.l2_bdaddr );

    // connect to target device
    status = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if(status!=0)
    {
        perror("Could not connect to dev");
        result = BLUE_ECONN;
        goto CLEAR;
    }

    bacpy(&cr->bdaddr, &addr.l2_bdaddr);
    cr->type = ACL_LINK;

    if ( ioctl(dd, HCIGETCONNINFO, (unsigned long)(cr)) < 0 )
    {
        perror("Could not get connection info");
        result = BLUE_ECONN;
        goto CLEAR;
    }


    if (hci_read_rssi(dd, htobs(cr->conn_info->handle), &readRSSI, 0) < 0)
    {
        perror("Could not read RSSI");
        result = BLUE_ECONN;
        goto CLEAR;
    }

    rssi = readRSSI;

CLEAR:
    free(cr);
    hci_close_dev(dd);
    close(sock);

    return result;
}

void CBlueAgent::stop()
{
    m_bContinue = false;
}

void CBlueAgent::onNotify(int fd, DevState state)
{
    auto devInfo = m_devList.find(fd);
    if( devInfo == m_devList.end())
    {
        //no dev found
    }
    else
    {
        //do nothing
    }

    COMM_PAKT send_buffer = {0};
    send_buffer.CMD = CMDType::SVR_NOTIFY;
    send_buffer.payload.STATE = state;

    write( fd, (void*)&send_buffer, sizeof(COMM_PAKT) );
}