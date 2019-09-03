#include <memory>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstdbool>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <regex>

#include <string>
#include <filesystem>
#include <rapidjson/document.h>

#include "helper.h"
#include "hciManager.h"


int main(int argc, char **argv)
{ 
  CHelper m_helper;
  
  int ch;
  std::string path = "";
  
  if(argc <1)
  {
    std::cout<<"Invalid Parameter..."<<std::endl;
  }
 
  while ((ch = getopt(argc, argv, "c:h?")) != -1)
  {
    switch (ch) {
      case 'c':
      {
        path = optarg;
        break;
      }
    }
  }
  
  if(path.at(0) == '/')
  {
    
  }
  else
  {
    std::string cwd = std::filesystem::current_path();
    path = cwd + '/' + path;
  }

  std::cout<< path << std::endl;
  struct config conf = m_helper.ParseConfig(path);

  std::function<void (std::string& info)> NilFunc = [&] (std::string& info)
  {
    std::string message = "";
    char text[4096] = {0};
    std::string name = conf.devices[info];

    std::cout<< name << ":" << info << " 未发现设备" <<std::endl;
    sprintf( text, MessageFormat.c_str(), name.c_str(), "未发现设备");
    message = text;
    CHelper::PostMessage(message, conf.users); 
  };

  std::function<void (std::string& info)> WeakFunc = [&] (std::string& info)
  {
    std::string message = "";
    char text[4096] = {0};
    std::string name = conf.devices[info];

    std::cout<< name << ":" << info << " 弱信号" <<std::endl;
    sprintf( text, MessageFormat.c_str(), name.c_str(), "弱信号");
    message = text;
    CHelper::PostMessage(message, conf.users); 
  };   

  std::function<void (std::string& info)> WithinFunc = [&] (std::string& info)
  {
    std::string message = "";
    char text[4096] = {0};
    std::string name = conf.devices[info];

    std::cout<< name << ":" << info << " 在家" <<std::endl;
    sprintf( text, MessageFormat.c_str(), name.c_str(), "在家");
    message = text;
    CHelper::PostMessage(message, conf.users); 
  };   

  std::function<void (std::string& info)> ErrorFunc = [&] (std::string& info)
  {
    std::string message = "";
    char text[4096] = {0};
    std::string name = conf.devices[info];

    std::cout<< name << ":" << info << " 错误" <<std::endl;
    sprintf( text, MessageFormat.c_str(), name.c_str(), "错误");
    message = text;
    CHelper::PostMessage(message, conf.users); 
  };   

  HciManager hci(NilFunc, WeakFunc, WithinFunc, ErrorFunc);
  hci.Init(0, 10);

  for(auto device : conf.devices)
  {
    hci.addDevice(device.first);
  }

  {
    std::time_t result = std::time(nullptr);
    
    hci.StartService();
  }
}
