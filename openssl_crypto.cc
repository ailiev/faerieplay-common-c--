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

/*
 * openssl_crypto.cc
 * alex iliev, may 2003
 */

#include <string>

#include <openssl/hmac.h>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/bio.h>

#include "sym_crypto.h"

#include "openssl_crypto.h"



using namespace std;


const EVP_CIPHER * DES3_CBC = EVP_des_ede3_cbc();
const EVP_CIPHER * DES3_ECB = EVP_des_ede3();



const EVP_MD * EVP_SHA1 = EVP_sha1();



//
// OSSLSymCrypto
//

OSSLSymCrypto::OSSLSymCrypto () throw (crypto_exception)
    : SymCryptProvider (EVP_CIPHER_iv_length (DES3_CBC),
			EVP_CIPHER_block_size (DES3_CBC)),
      _cipher           (DES3_CBC)
{
    EVP_CIPHER_CTX_init (&_context);
}



OSSLSymCrypto::~OSSLSymCrypto () {
    EVP_CIPHER_CTX_cleanup(&_context);
}


void OSSLSymCrypto::symcrypto_op (const ByteBuffer& input,
				  const ByteBuffer& key,
				  const ByteBuffer& iv,
				  ByteBuffer & out, OpType optype)
    throw (crypto_exception)
{
    if ( ! EVP_CipherInit_ex (&_context, _cipher, NULL, key.data(), iv.data(),
			      optype == ENCRYPT ? 1 : 0) )
    {
	throw crypto_exception
	    ( "Initializing " + get_op_name(optype) + " context failed: " +
	      make_ssl_error_report() );
    }
    

    byte * inbuf = input.data();
    int inlen = input.len();
    
    int outlen = 0;
    size_t running = 0;
    
    
    if ( ! EVP_CipherUpdate (&_context, out.data(), &outlen, inbuf, inlen) )
    {
	throw crypto_exception (get_op_name(optype) + " failed:" +
				make_ssl_error_report());
    }

    running += outlen;

    if ( ! EVP_CipherFinal_ex (&_context, out.data() + running, &outlen) ) {
	throw crypto_exception (get_op_name(optype) + " failed:" +
				make_ssl_error_report());
    }

    running += outlen;

    out.len() = running;
}








string make_ssl_error_report () {

    BIO* membio = BIO_new (BIO_s_mem());
    char * buf;

    ERR_print_errors (membio);
    int size = BIO_get_mem_ptr (membio, &buf);

    clog << "error report size = " << size << endl;

    string answer (buf, size);

    clog << "error report-->" << answer << "<--" << endl;

    BIO_free (membio);
    
    return answer;
}





//
// OSSL_HMAC
//

OSSL_HMAC::OSSL_HMAC () throw (crypto_exception)
    : MacProvider (HMAC_SHA1_MACSIZE),
      _md         (EVP_SHA1)
{
    HMAC_CTX_init (&_ctx);
}



OSSL_HMAC::~OSSL_HMAC () {
    HMAC_CTX_cleanup(&_ctx);
}


void OSSL_HMAC::genmac (const ByteBuffer& text, const ByteBuffer& key,
			ByteBuffer & out)
    throw (crypto_exception)
{
    size_t mac_len;
    
    HMAC_Init_ex (&_ctx, key.data(), key.len(), _md, NULL);
    HMAC_Update  (&_ctx, text.data(), text.len());
    HMAC_Final   (&_ctx, out.data(), &mac_len);

    if (mac_len > out.len()) {
	throw crypto_exception ("Out Buffer passed to OSSL_HMAC::genmac() "
				"is not large enough");
    }
    
    out.len() = mac_len;
}



