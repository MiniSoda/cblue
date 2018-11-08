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

    static int cipherText(unsigned char *cipheredBuff, const unsigned char *szText, size_t size);
    static int decipherText(unsigned char *decipheredBuff, const unsigned char *szText, size_t size);

    static void setEncrptKey(const char* szKey)
    {
        strcpy((char *)m_key, szKey);
    }

    static bool CheckAddr(const char *address);
    
  private:
    /* A 256 bit key */
    const static unsigned char m_key[128];
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
  };
};

enum ClientType { CREDENTIAL = 0, TRAYICON };
enum CMDType { CLI_REGISTER = 0, CLI_INQUIRE, SVR_NOTIFY };
enum DevState { OUTOFRANGE=0, WITHIN };


struct devInfo{
  int fd;
  unsigned char LISTEN_ADDR[30];
};