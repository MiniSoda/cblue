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
    //getchar();

    CBlueServer* pServer = &CBlueServer::getInstance();
    

    if( !m_helper.CheckAddr( strDevAddr.c_str() ))
    {
        cout<<"Bluetooth Address Invalid."<<endl;
    }
    else
    {
        pServer->InitServer();
        pServer->Run();
    }

}