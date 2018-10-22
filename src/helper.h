#include <string.h>

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>

class CHelper
{
  public:
    CHelper();
    ~CHelper();

    int cipherText(unsigned char *cipheredBuff, const unsigned char *szText, size_t size);
    int decipherText(unsigned char *decipheredBuff, const unsigned char *szText, size_t size);

    void setEncrptKey(const char* szKey)
    {
        strcpy((char *)m_key, szKey);
    }
  private:
    /* A 256 bit key */
    const unsigned char m_key[128];
    AES_KEY enc_key, dec_key;
};