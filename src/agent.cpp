#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <thread>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

#include "agent.h"


blueAgent::blueAgent()
{
    m_bContinue = true;
    m_state = DevState::UNKNOWN;
}

void blueAgent::addDevice(int fd, devInfo dev)
{
    if (fd <= 0)
    {
        //error handling
    }
    else
    {
        devNew.push(fd);
        devList.insert({fd,dev});
    }
}

void blueAgent::removeDevice(int fd)
{
    if (fd <= 0)
    {
        //error handling
    }
    else
    {
        devList.erase(fd);
    }
}

void blueAgent::run()
{
    while(m_bContinue)
    {

        vector<thread> threadList;
        while( devNew.size()!= 0 )
        {
            //add dev to listen queue
            int fd = devNew.top();
            //bluetooth address of the device
            if(devList.end() != devList.find(fd))
            {
                auto devMAC = devList.at(fd).LISTEN_ADDR;

                thread t([&]{
                    int rssi = -255;
                    GetRSSI(devMAC,rssi);
                });

                //threadList.push_back(t);
            }
            //remove
            devNew.pop();
        }
    }
}

int blueAgent::GetRSSI(const char *address, int& rssi)
{
    struct hci_conn_info_req *cr;
    
    int dd;
    int dev_id =0;
    int result = 0;  
    bool error = false;

    struct sockaddr_l2 addr = { 0 };
    int sock, status;
    DevState state = DevState::UNKNOWN;
    bool bStateChange = false;

    dd = hci_open_dev(dev_id);
    if (dd < 0)
    {
        perror("Could not open HCI device");
        result = BLUE_ERROR;
        //goto CLEAR;
    }

    if ((cr = (hci_conn_info_req *)malloc(sizeof(struct hci_conn_info_req) + sizeof(struct hci_conn_info))) == NULL)
    {
        perror("malloc");
        result = BLUE_ERROR;
        //goto CLEAR;
    }

    sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    addr.l2_family = AF_BLUETOOTH;
    //service discovery port
    addr.l2_psm = htobs(0x1);
    str2ba( address, &addr.l2_bdaddr );

    // connect to target device
    status = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if(status!=0)
    {
        perror("Could not connect to dev");
        result = BLUE_ECONN;
        //goto CLEAR;
    }

    bacpy(&cr->bdaddr, &addr.l2_bdaddr);
    cr->type = ACL_LINK;

    if ( ioctl(dd, HCIGETCONNINFO, (unsigned long)(cr)) < 0 )
    {
        perror("Could not get connection info");
        result = BLUE_ECONN;
        //goto CLEAR;
    }

    int count = 10;
    int8_t readRSSI = 0;

    while( count>0 )
    {
        if (hci_read_rssi(dd, htobs(cr->conn_info->handle), &readRSSI, 0) < 0)
        {
            perror("Could not read RSSI");
            result = BLUE_EREAD;
            goto CLEAR;
        }

        rssi = readRSSI;

        if( rssi < -10 )
        {
            if(state != DevState::OUTOFRANGE)
            {
                state = DevState::OUTOFRANGE;
                bStateChange = true;
                onNotify();
            }
            else
            {
                bStateChange = false;
            }
        }
        else
        {
            if(state != DevState::WITHIN)
            {
                state = DevState::WITHIN;
                bStateChange = true;
                onNotify();
            } 
            else
            {
                bStateChange = false;
            }
        }

        sleep(1);
        count--;
    }


CLEAR:
    free(cr);
    hci_close_dev(dd);
    close(sock);
    return result;
}

void blueAgent::stop()
{
    m_bContinue = false;
}