#include <chrono>
#include <memory>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstdbool>
#include <ctime>
#include <iostream>
#include <iomanip>
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
 
  while ((ch = getopt(argc, argv, "c:ht?")) != -1)
  {
    switch (ch) {
      case 'c':
      {
        path = optarg;
        break;
      }
      case 't':
      {
        //m_helper.isWeekday();
        m_helper.isWithinSchedule();
        exit(0);
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

  std::thread t([&](){
    bool Working = false;
    while(true)
    {
      bool schedule = m_helper.isWithinSchedule();
      bool weekday = m_helper.isWeekday(); 

      if( schedule && weekday )
      {
        if(Working == false)
        {
          hci.StartService();
          Working = true;
        }
      }
      else
      {
        auto now = std::chrono::system_clock::now(); 
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        tm local_tm = *localtime(&now_c);
        std::cout<< std::setfill('0') <<std::setw(2) <<local_tm.tm_hour<< ":" << local_tm.tm_min << ":" << local_tm.tm_sec << " off the schedule" << std::endl;

        hci.Stop();
        Working = false;
      }

      std::this_thread::sleep_for(std::chrono::seconds(conf.Interval));
    }  
  });

  if(t.joinable())
    t.join();    
}
