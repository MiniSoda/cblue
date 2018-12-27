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
#include "agent.h"
#include "BlueServer.h"

using namespace std;

//#define HCI_MAX_DEV 64

int error = 0;
bool bMainLoop = true;

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
        cout<<"Invalid Parameter..."<<endl;
    }

    bool bServer = false;
    bool bClient = false;
   
    while ((ch = getopt(argc, argv, "m:b:dvh?")) != -1)
    {
        switch (ch) {
            case 'b':
            {
                bClient = true;
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
            case 'd':
            {
                bServer = true;
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
    //getchar();

    
    
    if( bClient && bServer )
    {
        cout<<"Wrong params."<<endl;
    }

    if( bClient )
    {
        if( !m_helper.CheckAddr( strDevAddr.c_str() ))
        {
            cout<<"Bluetooth Address Invalid."<<endl;
        }
        else
        {
            CBlueAgent* pAgent = &CBlueAgent::getInstance();
            
            int nRet = 0;
            int fd = 1;
            DEVICE_HANDLE dev_handle = { 0 };

            while(true)
            {
  
                        
            }
            
        }
    }
    else if( bServer )
    {
        CBlueServer* pServer = &CBlueServer::getInstance();
        pServer->InitServer();
        pServer->Run();
    }

}