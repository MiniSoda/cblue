#include <memory>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstdbool>
#include <iostream>
#include <unistd.h>
#include <regex>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

#include "helper.h"
#include "hciManager.h"

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
            case 'c':
            {
                std::string config;
                config = optarg;
                m_helper.ParseConfig(config);
                
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

    std::function<void (std::string& info)> oneFunc = [] (std::string& info){
        cout<< "Nil" << endl;
        cout<< info << endl;
    };   

    std::function<void (std::string& info)> twoFunc = [] (std::string& info){
        cout<< "Weak" << endl;
        cout<< info << endl;
    };   

    std::function<void (std::string& info)> threeFunc = [] (std::string& info){
        cout<< "Within" << endl;
        cout<< info << endl;
    };   

    std::function<void (std::string& info)> fourFunc = [] (std::string& info){
        cout<< "Error" << endl;
        cout<< info << endl;
    };   

    HciManager hci(oneFunc,twoFunc,threeFunc,fourFunc);
    hci.Init(0, 10);
    hci.addDevice("F0:99:B6:92:84:E9");
    hci.addDevice("E0:AC:CB:EF:BC:B9");
    //hci.SetPollInterval(10);

    hci.StartService();
}