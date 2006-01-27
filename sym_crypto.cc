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
 * sym_crypto.cc
 * alex iliev, nov 2002
 */

#include <iostream>


/*
#include <fstream.h>
#include <strstream>
#include <ios>
*/


#include <common/utils.h>

#include <sym_crypto.h>


using namespace std;



const size_t DES_MAC_SIZE = 8;
const size_t DES3_KEY_SIZE = 3*8;

const size_t DES_BLOCK_SIZE = 8; // bytes



#ifdef TESTING_SYM_CRYPTO

#include <fstream>
#include <unistd.h>		// for write(2)
#include "openssl_crypto.h"

void usage (int argc, char * argv[]) {
    cerr << "usage: " << argv[0]
	 << " encrypt|decrypt|keygen"
	 << " <keyfile> <mackeyfile>" << endl
	 << "\tkeeps ciphertext in cipher.bin and mac.bin" << endl;
}    

int main (int argc, char * argv[]) {

    CryptoProviderFactory * prov_fact = new OpenSSLCryptProvFactory ();

    ByteBuffer key	(prov_fact->keysize());
    ByteBuffer mackey	(prov_fact->keysize());
    
    if (argc < 4) {
	usage (argc, argv);
	exit (1);
    }
    
    string op = argv[1];
    string keyfile = argv[2];
    string mackeyfile = argv[3];
    
    
    if (op == "keygen") {
	ByteBuffer
	    key (prov_fact->keysize()),
	    mackey (prov_fact->mac_keysize());

	auto_ptr<RandProvider> rands = prov_fact->getRandProvider();
	rands->randbytes (key);
	rands->randbytes (mackey);

	writefile (keyfile, key);
	writefile (mackeyfile, mackey);
	
	return 0;
    }

    // read in the keys
    key = readfile (keyfile);
    mackey = readfile (mackeyfile);
    

    // set up the crypto objects
    SymDencrypter denc ( prov_fact->getSymCryptProvider(),
			 prov_fact->getRandProvider(),
			 key );
    MacExpert maccer ( prov_fact->getMacProvider(),
		       prov_fact->getRandProvider(),
		       mackey );


    try {

	if (op == "encrypt") {
	    // read in the file contents
	    string os_string;
	    readfile (stdin, os_string);
	    ByteBuffer orig (os_string);
	    
	    // encrypt
	    ByteBuffer cipher, mac;
	    cipher = denc.encrypt (orig);
	    mac = maccer.genmac (cipher);

	    // and write out
	    writefile ("cipher.bin", cipher);
	    writefile ("mac.bin", mac);
	}
	else if (op == "decrypt") {
	    ByteBuffer
		mac = readfile ("mac.bin"),
		cipher = readfile ("cipher.bin"),
		answer;
	    
	    if ( ! maccer.checkmac (cipher, mac) ) {
		cerr << "MAC check failed!!" << endl;
		return 1;
	    }
	    
	    answer = denc.decrypt (cipher);
	    cout.write (answer.cdata(), answer.len());
	}
	else {
	    usage (argc, argv);
	    exit (1);
	}
    }
    catch (crypto_exception ex) {
	cerr << "Crypto failure: " << string (ex) << endl;
    }
    
}

#endif // TESTING_SYM_CRYPTO




//
// class SymWrapper
//

SymWrapper::SymWrapper (const ByteBuffer& key,
                        const ByteBuffer& mackey,
                        CryptoProviderFactory * provfact)
    throw (crypto_exception)
    : _denc         (provfact->getSymCryptProvider(), provfact->getRandProvider(), key),
      _maccer       (provfact->getMacProvider(), provfact->getRandProvider(), mackey),
      _do_mac       (true)
{}


// user should call keygen() sometime
SymWrapper::SymWrapper (CryptoProviderFactory * fact)
    throw (crypto_exception)
    : _denc     (fact->getSymCryptProvider(),	fact->getRandProvider()),
      _maccer   (fact->getMacProvider(),	fact->getRandProvider()),
      _do_mac   (true)
{
    // TODO: not sure if this makes sense to do here every time, maybe
    // the caller wants to set his own keys later on
    genkeys ();
}


void
SymWrapper::wrap (const ByteBuffer& cleartext, ByteBuffer & o_wrapped)
{

    if (o_wrapped.len() < wraplen (cleartext.len())) {
	throw bad_arg_exception ("SymWrapper::wrap: output buffer too small");
    }

    //
    // we'll stick the MAC in the start of the wrapped blob, followed by the
    // ciphertext
    //
    
    // aliases into o_wrapped
    ByteBuffer mac    (o_wrapped, 0,                _maccer.maclen());
    ByteBuffer cipher (o_wrapped, mac.len(),
		       _denc.cipherlen(cleartext.len()));

    _denc.encrypt (cleartext, cipher);
    _maccer.genmac (cipher, mac);

    o_wrapped.len() = mac.len() + cipher.len();
}



void
SymWrapper::unwrap (const ByteBuffer& wrapped, ByteBuffer & o_clear)
{
    size_t origlen = unwraplen (wrapped.len());

    if (o_clear.len() < origlen) {
	throw bad_arg_exception ("SymWrapper::unwrap: output buffer len "
				 + itoa(o_clear.len()) + " too small for "
				 + itoa(origlen));
    }

    
    // aliases into wrapped
    ByteBuffer mac    (wrapped, 0,                _maccer.maclen());
    ByteBuffer cipher (wrapped, mac.len(), wrapped.len() - mac.len());


    if (_do_mac && !_maccer.checkmac (cipher, mac)) {
	throw mac_failure_exception();
    }

    _denc.decrypt (cipher, o_clear);
    // HACK: skip the MAC by calling decrypt on the whole 'wrapped'
    //    _denc.decrypt (wrapped, o_clear);
}



ByteBuffer
SymWrapper::wrap (const ByteBuffer& cleartext)
{
    size_t outlen = wraplen (cleartext.len());
    ByteBuffer answer (outlen);
    wrap (cleartext, answer);
    return answer;
}


ByteBuffer
SymWrapper::unwrap (const ByteBuffer& wrapped)
{
    size_t outlen = unwraplen (wrapped.len());
    ByteBuffer answer (outlen);
    unwrap (wrapped, answer);
    return answer;
}

//
// and key manipulation routines
//
void SymWrapper::setkey (const ByteBuffer& key) throw ()
{
    _denc.setkey (key);
}

void SymWrapper::setmackey (const ByteBuffer& key) throw ()
{
    _maccer.setkey (key);
}

ByteBuffer SymWrapper::getkey () throw ()
{
    return _denc.getkey ();
}

ByteBuffer SymWrapper::getmackey () throw ()
{
    return _maccer.getkey ();
}

void SymWrapper::genkeys () throw (crypto_exception)
{
    _denc.genkey();
    _maccer.genkey();
}




//
//
// class SymDencrypter
//
//

// if initialized without a key, need to use the functions where a key
// is provided
SymDencrypter::SymDencrypter (std::auto_ptr<SymCryptProvider>  	op,
			      std::auto_ptr<RandProvider>	rand)
    throw ()
    : _op  (op),
      _rand(rand)
{}


SymDencrypter::SymDencrypter (std::auto_ptr<SymCryptProvider>  	op,
			      std::auto_ptr<RandProvider>	rand,
			      const ByteBuffer& 		key)
    throw ()
    : _key (key),
      _op  (op),
      _rand(rand)
{}


//SymDencrypter::~SymDencrypter () {}


void
SymDencrypter::encrypt (const ByteBuffer& cleartext, ByteBuffer & o_cipher,
			const ByteBuffer& key)
{

    // use a pseudo-random iv.
    ByteBuffer iv (_op->IVSIZE);
    _rand->randbytes (iv);
    
    // this would take too long to do every time i think, so ditch for
    // now, and just zero it out
    // TODO: could use a counter perhaps?
//    RAND_bytes (iv, IVSIZE);
//    memset (iv, 0, _op->IVSIZE);

    if (o_cipher.len() < cipherlen (cleartext.len())) {
	throw bad_arg_exception
	    ("SymDencrypter::encrypt: output buffer too small");
    }
    
    // save the IV at the start of the output buffer
    bbcopy (o_cipher, iv);

    // an alias, IVSIZE bytes into 'o_cipher', for the actual ciphertext
    ByteBuffer ciphertext (o_cipher, iv.len(),
			   o_cipher.len() - iv.len());
    
    // get the ciphertext
    _op->symcrypto_op (cleartext, key, iv, ciphertext, CRYPT_ENCRYPT);
    
    // ssl_symcrypto_op sets the length of the ciphertext, and we need to add
    // the iv length
    o_cipher.len() = ciphertext.len() + iv.len();

}


ByteBuffer
SymDencrypter::encrypt (const ByteBuffer& cleartext)
{
    size_t outlen = cipherlen (cleartext.len());
    ByteBuffer answer (outlen);
    encrypt (cleartext, answer);
    return answer;
}



void
SymDencrypter::decrypt (const ByteBuffer& enc, ByteBuffer & o_clear,
			const ByteBuffer& key)
{
    // need to:
    // retrieve the IV from front of buffer
    // call the generic function

    // aliases into cleartext:
    ByteBuffer iv         (enc, 0,      _op->IVSIZE);                 // the IV
    ByteBuffer ciphertext (enc, iv.len(), enc.len() - iv.len()); // the rest

    
    if (o_clear.len() < clearlen (ciphertext.len())) {
	throw bad_arg_exception
	    ("SymDencrypter::decrypt: output buffer len "
	     + itoa (o_clear.len()) + " too small for "
	     + itoa (clearlen (ciphertext.len())) );
    }
    

    _op->symcrypto_op (ciphertext, key, iv, o_clear,
		      CRYPT_DECRYPT);

    // the length of 'o_clear' will have been set by symcrypto_op()

}


ByteBuffer
SymDencrypter::decrypt (const ByteBuffer& enc)
{
    size_t outlen = clearlen (enc.len());
    ByteBuffer cleartext (outlen);
    decrypt (enc, cleartext);
    return cleartext;
}

void
SymDencrypter::setkey (const ByteBuffer& key) throw ()
{
    _key = key;
}

ByteBuffer
SymDencrypter::getkey () throw ()
{
    return _key;
}

void SymDencrypter::genkey () throw (crypto_exception)
{
    _key = ByteBuffer (_op->KEYSIZE);
    _rand->randbytes (_key);
}


SymDencrypter::~SymDencrypter ()
{
}





//
// SymCryptProvider
//

string get_crypt_op_name (crypt_op_name_t op) {
    switch (op) {
    case CRYPT_ENCRYPT:
	return "Encrypt";
    case CRYPT_DECRYPT:
	return "Decrypt";
    default:
	return "Mystery";
    }
}





//
// class BlockDencrypter
//


BlockDencrypter::BlockDencrypter (std::auto_ptr<BlockCryptProvider> prov,
				  std::auto_ptr<RandProvider>    randprov)
    throw (crypto_exception)
    : _prov (prov)
{
    assert (_prov.get() != NULL);
    assert (randprov.get() != NULL);

    _prov->setkey (randprov->alloc_randbytes (_prov->KEYSIZE));
}
    
void BlockDencrypter::encrypt (const ByteBuffer& in, ByteBuffer & out,
			       const ByteBuffer& key)
    throw (crypto_exception)
{
    _prov->setkey (key);
    _prov->crypt_op (in, out, CRYPT_ENCRYPT);
}



void BlockDencrypter::decrypt (const ByteBuffer& in, ByteBuffer & out)
    throw (crypto_exception)
{

    if (out.len() < _prov->BLOCKSIZE) {
	out = ByteBuffer (_prov->BLOCKSIZE);
    }

    _prov->crypt_op (in, out, CRYPT_DECRYPT);
}

