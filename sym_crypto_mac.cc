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

/*
 * sym_crypto_mac.cc
 * alex iliev, nov 2002
 */

#include <openssl/hmac.h>

#include "sym_crypto.h"
//#include "cbcmac.h"

#include <common/utils.h>


const EVP_MD * EVP_SHA1 = EVP_sha1();

const size_t HMAC_SHA1_KEYSIZE = 20; // 20 bytes, 160 bits, should work
				     // hopefully

const size_t HMAC_SHA1_MACSIZE = EVP_MD_size (EVP_SHA1);


MacExpert::MacExpert (const ByteBuffer& key)
    : _key     (key),
      _md      (EVP_SHA1),
      _macsize (EVP_MD_size(_md))
{
    HMAC_CTX_init (&_ctx);
}


MacExpert::~MacExpert () {
    HMAC_CTX_cleanup(&_ctx);
}


ByteBuffer
MacExpert::genmac (const ByteBuffer& text) throw (crypto_exception) {

    
    ByteBuffer answer (new byte[_macsize], _macsize);

    HMAC_Init_ex (&_ctx, _key.data(), _key.len(), _md, NULL);
    HMAC_Update  (&_ctx, text.data(), text.len());
    HMAC_Final   (&_ctx, answer.data(), &(answer.len()));
    
    return answer;
}


bool
MacExpert::checkmac(ByteBuffer text, ByteBuffer mac) throw (crypto_exception) {

    ByteBuffer this_mac = genmac (text);

    return
	this_mac.len() == this_mac.len() &&
	memcmp (this_mac.data(), mac.data(), mac.len()) == 0;
}
