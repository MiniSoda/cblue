#include "helper.h"
#include <rapidjson/document.h>


CHelper::CHelper()
{
    //aes encrpt
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

//this key should read from config
strcpy((char *)m_key, "temporary key");

    AES_set_encrypt_key(m_key, 128, &enc_key);
    AES_set_decrypt_key(m_key, 128, &dec_key);
}



CHelper::~CHelper()
{

}

int CHelper::cipherText(unsigned char *cipheredBuff, const unsigned char *szText, size_t size = 0)
{
     AES_encrypt(szText, cipheredBuff, &enc_key);
     int length = (size/16 + 1) * 16;
     return length;
}

int CHelper::decipherText(unsigned char *decipheredBuff, const unsigned char *szText, size_t size = 0)
{
    AES_decrypt(szText, decipheredBuff, &dec_key);
    int length = size-16;
    return length;
}

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
        for( auto& item : itr->value.GetArray() ) 
        {
          conf.devices.emplace_back(item.GetString());
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
    }
  }
  
  fclose(fp);

  return conf;
}


bool CHelper::PostMessage(const std::string& message)
{
  CURL *curl;
  CURLcode res;

  char raw_message[500] = {0};
  sprintf( raw_message, 
            R"(payload={"text": "%s", "user_ids": [4] })" , 
            message.c_str());

  std::string payload = raw_message;

  /* In windows, this will init the winsock stuff */ 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* get a curl handle */ 
  curl = curl_easy_init();
  if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */ 
    curl_easy_setopt(curl, CURLOPT_URL, "https://nas.imangoo.site:5001/webapi/entry.cgi");
    //api=SYNO.Chat.External&method=chatbot&version=2&token=%22BrY2JNYksVRYvA0Pm9TAoVvLWt3lhk7upvFT6SYuumiXG6MSqEqwotUNQg65Et8i%22
    std::string format = "api=SYNO.Chat.External&method=chatbot&version=2&token=%22BrY2JNYksVRYvA0Pm9TAoVvLWt3lhk7upvFT6SYuumiXG6MSqEqwotUNQg65Et8i%22&";
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
