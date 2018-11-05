#include <sys/ioctl.h>
#include <sys/socket.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <unistd.h>
#include <regex>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

#include "helper.h"

using namespace std;

//#define HCI_MAX_DEV 64

#define BLUE_ERROR 2

#define BLUE_NOT_CONNECTED -1

int error = 0;
bool bMainLoop = true;

bool CheckAddr(const char *address)
{
    bool bCheck = false;

    std::string s(address);
    std::regex e("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$");
    
    if (std::regex_match (s,e))
        bCheck = true;

    return bCheck;
}

int GetRSSI(const char *address)
{

    struct hci_conn_info_req *cr;
    int8_t rssi;
    int dd;
    int dev_id =0;
    
    error = false;

    struct sockaddr_l2 addr = { 0 };
    int sock, status;

    dd = hci_open_dev(dev_id);
    if (dd < 0)
    {
        perror("Could not open HCI device");
        return BLUE_ERROR;
    }

    if ((cr = (hci_conn_info_req *)malloc(sizeof(struct hci_conn_info_req) + sizeof(struct hci_conn_info))) == NULL)
    {
        perror("malloc");
        hci_close_dev(dd);
        return BLUE_ERROR;
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
        free(cr);
        hci_close_dev(dd);
        close(sock);
        return BLUE_ERROR;
    }

    bacpy(&cr->bdaddr, &addr.l2_bdaddr);
    cr->type = ACL_LINK;

    if (ioctl(dd, HCIGETCONNINFO, (unsigned long)cr) < 0)
    {
        perror("Could not get connection info");
        free(cr);
        hci_close_dev(dd);
        close(sock);
        return BLUE_ERROR;
    }

    int count = 10;
    while( count>0 )
    {
        if (hci_read_rssi(dd, htobs(cr->conn_info->handle), &rssi, 0) < 0)
        {
            perror("Could not read RSSI");
            free(cr);
            hci_close_dev(dd);
            close(sock);
            return BLUE_ERROR;
        }
        cout <<"rssi: " << int(rssi) << endl;
        sleep(1);
        count--;
    }

    free(cr);
    hci_close_dev(dd);
    close(sock);
    return rssi;
}

std::string g_strEncrptKey;

int main(int argc, char **argv)
{ 
    CHelper m_helper;
    g_strEncrptKey = "";
    char* pszEnc = nullptr;
    char* pszDev = nullptr;

    int ch;
    std::string strDevAddr ="";
    
    
    if(argc <1)
    {
        cout<<"Please Input Bluetooth Address..."<<endl;
    }
   
    while ((ch = getopt(argc, argv, "m:b:vh?")) != -1)
    {
        switch (ch) {
            case 'b':
            {
                strDevAddr = optarg;
                if(strDevAddr[0] == ' ')
                {
                    strDevAddr = strDevAddr.substr(1,strDevAddr.length());
                }
                break;
            }
            case 'm':
            {
                g_strEncrptKey = optarg;
                if(g_strEncrptKey[0] == ' ')
                {
                    g_strEncrptKey = g_strEncrptKey.substr(1,g_strEncrptKey.length());
                }
                break;
            }
        }
    }

    m_helper.setEncrptKey(g_strEncrptKey.c_str());

    unsigned char szText1[128] = {0};
    unsigned char szText2[128] = {0};

    unsigned char szGreetings[30] = "Hello Cipher!";
    m_helper.cipherText( szText1, szGreetings, 13);
    m_helper.decipherText( szText2, szText1, 13);
    
    printf("%s", szText2);
    getchar();


    if( !CheckAddr( strDevAddr.c_str() ))
    {
        cout<<"Bluetooth Address Invalid."<<endl;
    }
    else
    {
        while(bMainLoop)
            GetRSSI(strDevAddr.c_str());
    }

}