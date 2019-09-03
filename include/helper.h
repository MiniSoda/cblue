#include <cstring>

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>
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

    int cipherText(unsigned char *cipheredBuff, const unsigned char *szText, size_t size);
    int decipherText(unsigned char *decipheredBuff, const unsigned char *szText, size_t size);

    void setEncrptKey(const char* szKey)
    {
        strcpy((char*)m_key, szKey);
    }

    bool CheckAddr(const char *address);

    config ParseConfig(std::string config);
    bool isWithinSchedule( );

    static bool isWithinSchedule( std::chrono::system_clock::time_point s, std::chrono::system_clock::time_point e);

    static bool PostMessage(const std::string& message, std::string userid);
    static bool PostMessage(const std::string& message, std::vector<int> userids);
    
  private:
    /* A 256 bit key */
    unsigned char m_key[128];
    AES_KEY enc_key, dec_key;
    rapidjson::Document m_config;
    static std::string connectionString;
    static std::string token;
    std::string startTime;
    std::string endTime;
};
