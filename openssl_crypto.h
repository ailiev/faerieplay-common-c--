/*  -*- c++ -*-
 * ** PIR Private Directory Service prototype
 * ** Copyright (C) 2003 Alexander Iliev <iliev@nimbus.dartmouth.edu>
 * **
 * ** This program is free software; you can redistribute it and/or modify
 * ** it under the terms of the GNU General Public License as published by
 * ** the Free Software Foundation; either version 2 of the License, or
 * ** (at your option) any later version.
 * **
 * ** This program is distributed in the hope that it will be useful,
 * ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 * ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * ** GNU General Public License for more details.
 * **
 * ** You should have received a copy of the GNU General Public License
 * ** along with this program; if not, write to the Free Software
 * ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * */

/*
 * openssl_crypto.h
 * alex iliev, may 2003
 */

#include <openssl/evp.h>
#include <openssl/hmac.h>

#include "randomsrc.h"
#include "sym_crypto.h"


class OSSLSymCrypto : public SymCryptProvider {

public:

    OSSLSymCrypto ();
    
    virtual void symcrypto_op (const ByteBuffer& input,
			       const ByteBuffer& key,
			       const ByteBuffer& iv,
			       ByteBuffer & out,
			       crypt_op_name_t optype)
	throw (crypto_exception);

    virtual ~OSSLSymCrypto();

private:
    EVP_CIPHER_CTX _context;
    const EVP_CIPHER * _cipher;

};


#if 0
class OSSLBlockCrypt : public BlockCryptProvider {

public:

    OSSLBlockCrypt ();

    virtual void crypt_op (const ByteBuffer& in,
			   ByteBuffer & out,
			   const ByteBuffer& key,
			   crypt_op_name_t op)
	throw (crypto_exception);

private:

}
#endif


class OSSL_HMAC : public MacProvider {

public:

    OSSL_HMAC ();

    
    virtual void genmac (const ByteBuffer& text, const ByteBuffer& key,
			 ByteBuffer & out)
	throw (crypto_exception);

    ~OSSL_HMAC();


private:
    
    HMAC_CTX _ctx;
    const EVP_MD * _md;
    
};



class OSSL_SHA1 : public HashProvider {

    public:

    OSSL_SHA1 ();
    
    // for producing just a single hash, leaving the object in an initial state
    // again
    virtual void singleHash (const ByteBuffer& bytes, ByteBuffer & o_hash)
	throw (crypto_exception);

    // and these are for generating a hash in stages
    virtual void initMultiple() throw (crypto_exception);
    virtual void addBytes(const ByteBuffer& bytes)
	throw (crypto_exception);
    // getHash() leaves the object in an initial state
    virtual void getHash(ByteBuffer & o_hash) throw (crypto_exception);


    virtual ~OSSL_SHA1();

private:
    EVP_MD_CTX _ctx;
    const EVP_MD * _md;		// the message digest algorithm
};




//
// class OpenSSLRandProvider
//
class OpenSSLRandProvider : public RandProvider {
    
    virtual void randbytes (ByteBuffer & out) throw (crypto_exception);

};




//
// and our factory class
//
class OpenSSLCryptProvFactory : public CryptoProviderFactory {

    virtual std::auto_ptr<SymCryptProvider> getSymCryptProvider()
	throw (crypto_exception)
	{
	    return std::auto_ptr<SymCryptProvider> (new OSSLSymCrypto ());
	}

    
    virtual std::auto_ptr<MacProvider>      getMacProvider()
	throw (crypto_exception)
	{
	    return std::auto_ptr<MacProvider> (new OSSL_HMAC ());
	}
	
    virtual std::auto_ptr<HashProvider>     getHashProvider()
	throw (crypto_exception)
	{
	    return std::auto_ptr<HashProvider> (new OSSL_SHA1());
	}

    virtual std::auto_ptr<RandProvider>     getRandProvider()
	throw (crypto_exception)
	{
	    return std::auto_ptr<RandProvider> (new OpenSSLRandProvider ());
	}

};




extern const EVP_CIPHER * DES3_CBC;
extern const EVP_CIPHER * DES3_ECB;

extern const EVP_MD * EVP_SHA1;


//
// helper to generate an error message
//
std::string make_ssl_error_report ();

