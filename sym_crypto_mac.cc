/*
 * Library routines for the Dartmouth CS Private Information Retrieval and
 * Writing (PIR/W) library.
 *
 * Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu>
 *
 * All rights reserved.
 *
 * This code is released under a BSD license.
 * Please see LICENSE.txt for the full license and disclaimers.
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com)
*/

/*
 * sym_crypto_mac.cc
 * alex iliev, nov 2002
 */

#include "sym_crypto.h"
//#include "cbcmac.h"

#include <common/utils.h>

const size_t HMAC_SHA1_KEYSIZE = 20; // 20 bytes, 160 bits, should work
				     // hopefully


const size_t HMAC_SHA1_MACSIZE = 20; // 20 bytes / 160 bits hash



using namespace std;



//
// MacExpert
//

MacExpert::MacExpert (auto_ptr<MacProvider> 	    op,
		      std::auto_ptr<RandProvider>   rand,
		      const ByteBuffer& 	    key)
    : _key	(key),
      _op	(op),
      _rand	(rand)
{}


MacExpert::MacExpert (auto_ptr<MacProvider> 	    op,
		      std::auto_ptr<RandProvider>   rand)
    : _op	(op),
      _rand	(rand)
{}


void
MacExpert::genmac (const ByteBuffer& text, ByteBuffer & out)
{
    _op->genmac (text, _key, out);
}


ByteBuffer
MacExpert::genmac (const ByteBuffer& text)
{
    ByteBuffer mac (new byte[_op->MACSIZE], _op->MACSIZE);
    genmac (text, mac);
    return mac;
}


bool
MacExpert::checkmac(const ByteBuffer& text, const ByteBuffer& mac)
{

    ByteBuffer this_mac = genmac (text);

    return
	mac.len() == this_mac.len()
	&&
	memcmp (this_mac.data(), mac.data(), mac.len()) == 0;
}


void
MacExpert::setkey (const ByteBuffer& key) throw ()
{
    _key = key;
}

ByteBuffer
MacExpert::getkey () throw ()
{
    return _key;
}

void MacExpert::genkey () throw (crypto_exception)
{
    // reallocate and generate
    _key = ByteBuffer (_op->KEYSIZE);
    _rand->randbytes (_key);
}
