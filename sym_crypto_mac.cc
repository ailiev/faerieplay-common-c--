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

#include "sym_crypto.h"
#include "cbcmac.h"

#include <common/utils.h>


MacExpert::MacExpert (const ByteBuffer& key)
    : _key (key)
{
    EVP_CIPHER_CTX_init (&(_ctx.cctx));
}

MacExpert::~MacExpert () {
    EVP_CIPHER_CTX_cleanup (&(_ctx.cctx));
}


ByteBuffer
MacExpert::genmac (const ByteBuffer& text) throw (crypto_exception) {

    
    ByteBuffer answer (new byte[DES_MAC_SIZE], DES_MAC_SIZE);
    
    // note: using EBC here as the code in cbcmac.c uses it to construct a CBC
    // mode better suited to MACs (not to generate the whole encryption of the
    // input, but just one block)
    
    int macsize = 0;
    
    // FIXME: the error condition may change when i fix cbcmac.c
    if ( CBCMAC (DES3_ECB,
		 _key.data(), _key.len(),
		 text.data(), text.len(),
		 answer.data(), &macsize) < 0 )
    {
	throw crypto_exception ("Generating MAC failed: " +
				make_ssl_error_report());
    }

    // TODO: will the macsize be what we expect (ie DES_MAC_SIZE)?

    return answer;
}


bool
MacExpert::checkmac(ByteBuffer text, ByteBuffer mac) throw (crypto_exception) {

    ByteBuffer this_mac = genmac (text);

    return
	this_mac.len() == this_mac.len() &&
	memcmp (this_mac.data(), mac.data(), mac.len()) == 0;
}



int
MacExpert::CBCMAC (const EVP_CIPHER * c, const unsigned char *key, int key_len,
		   const unsigned char *str, int sz,
		   unsigned char *out, int *outlen)
{
  int e;

  if ((e = CBCMAC_Init (&_ctx, c, key)))
    return e;
  if ((e = CBCMAC_Update (&_ctx, reinterpret_cast<const char*> (str), sz)))
    return e;
  e = CBCMAC_Final (&_ctx, out, outlen);


  return e;
}
