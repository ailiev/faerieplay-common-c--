/*
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

/* -*- C++ -*-
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

    ByteBuffer _key;
    
public:
    
    SymDencrypter (const ByteBuffer& key);

    
    ByteBuffer
    encrypt (const ByteBuffer& cleartext)
	throw (crypto_exception);

    ByteBuffer
    decrypt (const ByteBuffer& ciphertext)
	throw (crypto_exception);

};



class MacExpert {

private:
    ByteBuffer _key;
    
public:
    
    MacExpert (const ByteBuffer& key);

    ByteBuffer
    genmac (const ByteBuffer& text)
	throw (crypto_exception);

    bool
    checkmac(ByteBuffer text, ByteBuffer mac)
	throw (crypto_exception);

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


// a little helper function to run an openssl EVP operation from start to end
ByteBuffer
ssl_symcrypto_op
(const ByteBuffer& input,
 const ByteBuffer& key,
 EVP_CIPHER_CTX *ctx,
 int (*init)   (EVP_CIPHER_CTX*, const EVP_CIPHER *, byte *, byte *),
 int (*update) (EVP_CIPHER_CTX *, byte *, int *, byte *, int ),
 int (*final) (EVP_CIPHER_CTX *, byte *, int *),
 const std::string& name)
    throw (crypto_exception);

#endif // _SYM_CRYPTO_H
