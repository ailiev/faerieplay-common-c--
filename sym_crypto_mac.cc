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
    _rand->randbytes (_key);
}
