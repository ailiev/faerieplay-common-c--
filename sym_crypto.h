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

#include <common/exceptions.h>
#include <common/utils.h>

#include "cbcmac.h"


#ifndef _SYM_CRYPTO_H
#define _SYM_CRYPTO_H


class SymDencrypter {

private:
    EVP_CIPHER_CTX _context;
    const EVP_CIPHER * _cipher;

    ByteBuffer _key;

public:
    
    enum OpType { ENCRYPT, DECRYPT };
    
    SymDencrypter (const ByteBuffer& key) throw (crypto_exception);

    
    ByteBuffer
    encrypt (const ByteBuffer& cleartext)
	throw (crypto_exception);

    ByteBuffer
    decrypt (const ByteBuffer& ciphertext)
	throw (crypto_exception);

    ~SymDencrypter();


private:
    //
    // a little helper function to run an openssl EVP operation from start to
    // end
    //
    void ssl_symcrypto_op (const ByteBuffer& input, const ByteBuffer& iv,
			   ByteBuffer & out, OpType optype)
	throw (crypto_exception);

    const size_t IVSIZE, BLOCKSIZE;
};



class MacExpert {

private:
    
public:
    
    MacExpert (const ByteBuffer& key);

    ByteBuffer
    genmac (const ByteBuffer& text)
	throw (crypto_exception);

    bool
    checkmac(ByteBuffer text, ByteBuffer mac)
	throw (crypto_exception);

    ~MacExpert();

private:

    ByteBuffer _key;
    CBCMAC_CTX _ctx;

    int CBCMAC (const EVP_CIPHER * c, const unsigned char *key, int key_len,
		const unsigned char *str, int sz,
		unsigned char *out, int *outlen);
};



// defined in sym_crypto.cc
extern const size_t DES_MAC_SIZE; // in bytes
extern const size_t DES3_KEY_SIZE; // in bytes

extern const EVP_CIPHER * DES3_CBC;
extern const EVP_CIPHER * DES3_ECB;


//
// helper to generate an error message
//
std::string make_ssl_error_report ();






#endif // _SYM_CRYPTO_H
