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


