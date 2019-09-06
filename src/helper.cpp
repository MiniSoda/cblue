#include "helper.h"

#include <chrono>
#include <iomanip>
#include <ctime>
#include <limits>
#include <rapidjson/document.h>
#include <rapidjson/error/error.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <ratio>
#include <utility>

CHelper::CHelper()
{
    startTime = "";
    endTime = "";
}

CHelper::~CHelper()
{

}
std::string CHelper::connectionString = "";
std::string CHelper::token = "";

bool CHelper::CheckAddr(const char *address)
{
    bool bCheck = false;

    std::string s(address);
    std::regex e("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$");
    
    if (std::regex_match (s,e))
        bCheck = true;

    return bCheck;
}

config CHelper::ParseConfig(std::string config)
{
  struct config conf;
  FILE* fp = fopen(config.c_str(), "rb"); // non-Windows use "r"
  char readBuffer[65536];
  rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
  
  rapidjson::Document document;
  document.ParseStream<0, rapidjson::UTF8<>, rapidjson::FileReadStream>(is);
  if (document.HasParseError())
  {
    std::cout
    << "\n\nGetParseError=" << static_cast<int>(document.GetParseError())
    << " - (" << document.GetErrorOffset()
    << std::endl << std::endl;
  }
  else
  {

  }

  if(document.IsObject())
  {
    for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
    {
      if(itr->name.GetString() == DeviceLabel) 
      {
        for(rapidjson::Value::ConstMemberIterator itr1 = itr->value.MemberBegin(); itr1 != itr->value.MemberEnd(); ++itr1)
        {
          conf.devices.emplace( std::make_pair(itr1->name.GetString() ,itr1->value.GetString()) );
        }
      }

      if(itr->name.GetString() == ThresLabel)
      {
        conf.Threshold = itr->value.GetInt();
      }

      if(itr->name.GetString() == IntervalLabel) 
      {
        conf.Interval = itr->value.GetInt();
      }
      
      if(itr->name.GetString() == UsersLabel) 
      {
        for(auto& item : itr->value.GetArray())
        {
          conf.users.emplace_back(item.GetInt());
        }
      }

      if(itr->name.GetString() == ConnLabel) 
      {
        CHelper::connectionString = itr->value.GetString();
      }
      
      if(itr->name.GetString() == TokenLabel) 
      {
        CHelper::token = itr->value.GetString();
      }

      if(itr->name.GetString() == SchedLabel) 
      {
        conf.StartTime = itr->value.GetArray()[0].GetString();
        conf.EndTime = itr->value.GetArray()[1].GetString();

        startTime = conf.StartTime;
        endTime = conf.EndTime;

        int sh = stoi(startTime);
        int sm = stoi(startTime.substr(3));
        long start = sm + sh*60;

        int eh = stoi(endTime);
        int em = stoi(endTime.substr(3));
        long end = em + eh*60;

        m_start = start;        
        m_end = end;
      }
    }
  }

  fclose(fp);
  return conf;
}

volatile bool CHelper::isWithinSchedule( )
{
  volatile bool scheduled = false;
  auto now = std::chrono::system_clock::now(); 
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  tm local_tm = *localtime(&now_c);
  long h = local_tm.tm_hour;   
  long m = local_tm.tm_min;   
  long cur = h*60+m;
  
  if( cur >= m_start && cur < m_end )
  {
    scheduled = true;
  }

  return scheduled;
}

volatile bool CHelper::isWeekday( )
{
  volatile bool weekday = false;
  auto now = std::chrono::system_clock::now(); 
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  tm local_tm = *localtime(&now_c);
  if( local_tm.tm_wday >=1 && local_tm.tm_wday <=5 )
    weekday = true;
  return weekday;
}

bool CHelper::PostMessage(const std::string& message, std::vector<int> userids)
{
  rapidjson::Document document;
  rapidjson::Value list(rapidjson::kArrayType);
  rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
  for (auto id : userids)
    list.PushBack(id, allocator);   // allocator is needed for potential realloc(). 

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  list.Accept(writer);
  const char* output = buffer.GetString();
  return CHelper::PostMessage( message, output );
}

bool CHelper::PostMessage(const std::string& message, std::string userid)
{
  CURL *curl;
  CURLcode res;

  char raw_message[500] = {0};
  sprintf( raw_message, 
            R"(payload={"text": "%s", "user_ids": %s })" , 
            message.c_str(), userid.c_str());

  std::string payload = raw_message;

  /* In windows, this will init the winsock stuff */ 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* get a curl handle */ 
  curl = curl_easy_init();
  if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */ 
    curl_easy_setopt(curl, CURLOPT_URL, connectionString.c_str());
    sprintf( raw_message, "api=SYNO.Chat.External&method=chatbot&version=2&token=%s&", token.c_str());
    std::string format = raw_message;
    format += payload; 

    /* Now specify the POST data */ 
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, format.c_str());
 
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();  
  return true;
}
