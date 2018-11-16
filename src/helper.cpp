#include "helper.h"


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