/*
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

