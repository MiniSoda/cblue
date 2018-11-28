#include <string.h>

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>

#include <regex>

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
    
  private:
    /* A 256 bit key */
    unsigned char m_key[128];
    AES_KEY enc_key, dec_key;
};

struct COMM_PAKT{
  unsigned char HEADER[4];
  unsigned char CMD;
  unsigned char res[3];

  struct PAYLOAD{
    unsigned char CLIENT_TYPE;
    unsigned char STATE;
    unsigned char LISTEN_ADDR[30];
  }payload;
};

enum ClientType { CREDENTIAL = 0, TRAYICON };
enum CMDType { CLI_REGISTER = 0, CLI_INQUIRE, SVR_NOTIFY, ACK };
enum DevState { OUTOFRANGE=0, WITHIN, UNKNOWN };


struct devInfo{
  
  int fd;
  char LISTEN_ADDR[30];
  DevState state;

  devInfo()
  {
    fd =0;
    memset( LISTEN_ADDR,0,30);
    state = DevState::UNKNOWN;
  }
};