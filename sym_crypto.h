/* -*- C++ -*-
 * ** PIR Private Directory Service prototype
 * ** Copyright (C) 2002 Alexander Iliev <iliev@nimbus.dartmouth.edu>
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
 * sym_crypto.h
 * alex iliev, nov 2002
 * declarations for symmetric crypto utility classes
 */

#include <string>

#include <openssl/evp.h>
#include <openssl/hmac.h>

#include <common/exceptions.h>
#include <common/utils.h>

//#include "cbcmac.h"


#ifndef _SYM_CRYPTO_H
#define _SYM_CRYPTO_H


class SymCryptoOperator {

public:
    
    enum OpType { ENCRYPT, DECRYPT };

    SymCryptoOperator (size_t IVSIZE, size_t BLOCKSIZE);
    
    virtual void symcrypto_op (const ByteBuffer& input,
			       const ByteBuffer& key,
			       const ByteBuffer& iv,
			       ByteBuffer & out,
			       OpType optype)
	throw (crypto_exception) = 0;

    virtual ~SymCryptoOperator ();

    const size_t IVSIZE, BLOCKSIZE;

protected:
        
    static std::string get_op_name (OpType op);
    
};




class OSSLSymCrypto : public SymCryptoOperator {

public:

    OSSLSymCrypto () throw (crypto_exception);
    
    virtual void symcrypto_op (const ByteBuffer& input,
			       const ByteBuffer& key,
			       const ByteBuffer& iv,
			       ByteBuffer & out,
			       SymCryptoOperator::OpType optype)
	throw (crypto_exception);

    virtual ~OSSLSymCrypto();

private:
    EVP_CIPHER_CTX _context;
    const EVP_CIPHER * _cipher;

};





class MacOperator {

public:

    MacOperator (size_t MACSIZE);
    
    virtual ByteBuffer genmac (const ByteBuffer& text, const ByteBuffer& key)
	throw (crypto_exception) = 0;

    virtual ~MacOperator();

    const size_t MACSIZE;
};



class OSSL_HMAC : public MacOperator {

public:

    OSSL_HMAC ();

    
    virtual ByteBuffer genmac (const ByteBuffer& text, const ByteBuffer& key)
	throw (crypto_exception);

    ~OSSL_HMAC();


private:
    
    HMAC_CTX _ctx;
    const EVP_MD * _md;
    
};






class SymDencrypter {



public:
    
    SymDencrypter (const ByteBuffer& key, SymCryptoOperator & op)
	throw (crypto_exception);

    
    ByteBuffer
    encrypt (const ByteBuffer& cleartext)
	throw (crypto_exception);

    ByteBuffer
    decrypt (const ByteBuffer& ciphertext)
	throw (crypto_exception);

    ~SymDencrypter();


private:
    
    ByteBuffer _key;
    SymCryptoOperator & _op;
};





class MacExpert {

public:
    
    MacExpert (const ByteBuffer& key, MacOperator & op);

    ByteBuffer
    genmac (const ByteBuffer& text)
	throw (crypto_exception);

    bool
    checkmac(ByteBuffer text, ByteBuffer mac)
	throw (crypto_exception);

private:

    const ByteBuffer _key;
    MacOperator & _op;

};



//
// all the following are in bytes
//

// defined in sym_crypto.cc
extern const size_t DES_MAC_SIZE;
extern const size_t DES3_KEY_SIZE;

// in sym_crypto_mac.cc
extern const size_t HMAC_SHA1_KEYSIZE;
extern const size_t HMAC_SHA1_MACSIZE;



extern const EVP_CIPHER * DES3_CBC;
extern const EVP_CIPHER * DES3_ECB;

extern const EVP_MD * EVP_SHA1;


//
// helper to generate an error message
//
std::string make_ssl_error_report ();






#endif // _SYM_CRYPTO_H
