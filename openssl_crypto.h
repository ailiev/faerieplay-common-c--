/* -*- c++ -*-
 * openssl_crypto.h
 * alex iliev, may 2003
 */

#include <openssl/evp.h>
#include <openssl/hmac.h>



class OSSLSymCrypto : public SymCryptProvider {

public:

    OSSLSymCrypto () throw (crypto_exception);
    
    virtual void symcrypto_op (const ByteBuffer& input,
			       const ByteBuffer& key,
			       const ByteBuffer& iv,
			       ByteBuffer & out,
			       SymCryptProvider::OpType optype)
	throw (crypto_exception);

    virtual ~OSSLSymCrypto();

private:
    EVP_CIPHER_CTX _context;
    const EVP_CIPHER * _cipher;

};





class OSSL_HMAC : public MacProvider {

public:

    OSSL_HMAC () throw (crypto_exception);

    
    virtual void genmac (const ByteBuffer& text, const ByteBuffer& key,
			 ByteBuffer & out)
	throw (crypto_exception);

    ~OSSL_HMAC();


private:
    
    HMAC_CTX _ctx;
    const EVP_MD * _md;
    
};


extern const EVP_CIPHER * DES3_CBC;
extern const EVP_CIPHER * DES3_ECB;

extern const EVP_MD * EVP_SHA1;


//
// helper to generate an error message
//
std::string make_ssl_error_report ();

