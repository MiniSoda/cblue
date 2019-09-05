#include <cstring>

#include <regex>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <chrono>

#include <curl/curl.h>

#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

#pragma once

const static std::string DeviceLabel = "device";
const static std::string UsersLabel = "userId";
const static std::string ThresLabel = "Threshold";
const static std::string IntervalLabel = "Interval";
const static std::string ConnLabel = "Connection";
const static std::string TokenLabel= "Token";
const static std::string SchedLabel= "ScheduleTime";

const static std::string MessageFormat= "推送消息: {%s} 的设备信号状态 {%s}";

struct config
{
  std::map<std::string,std::string> devices;
  std::vector<int> users;
  int Threshold;
  int Interval;

  std::string ConnectionString;
  std::string Token;
  std::string StartTime;
  std::string EndTime;

  config(){
    Threshold = 0;
    Interval = 10;

    ConnectionString = "";
    Token = "";
    StartTime = "";
    EndTime = "";
  }
};

class CHelper
{
  public:
    CHelper();
    ~CHelper();

    bool CheckAddr(const char *address);

    config ParseConfig(std::string config);
    volatile bool isWithinSchedule( );
    volatile bool isWeekday( );

    static bool PostMessage(const std::string& message, std::string userid);
    static bool PostMessage(const std::string& message, std::vector<int> userids);
    
  private:
    rapidjson::Document m_config;
    static std::string connectionString;
    static std::string token;
    std::string startTime;
    std::string endTime;

    long m_start;
    long m_end;
};
