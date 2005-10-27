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

#include<iostream>
#include<fstream>

// from the gcc-3 porting howto
#include <sstream>



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

// FIXME: this test driver is very out of date!

int main (int argc, char * argv[]) {

    byte key    [DES3_KEY_SIZE];
    byte mackey [DES3_KEY_SIZE];
    
    if (argc < 3) {
	cerr << "usage: " << argv[0]
	     << " encrypt|decrypt|keygen"
	     << " <keyfile> [<mackeyfile>]"
	     << endl;
	exit (1);
    }
    
    string op = argv[1];
    string keyfile = argv[2];
    
    ERR_load_crypto_strings();
    
    if (op == "keygen") {
	RAND_bytes (key, DES3_KEY_SIZE);

	FILE* keyfh = fopen (keyfile.c_str(), "w");
	fwrite (key, 1, DES3_KEY_SIZE, keyfh);
	fclose (keyfh);
	return 0;
    }

    string mackeyfile = argv[3];

    // read in the key
    FILE* keyfh = fopen (keyfile.c_str(), "r");
    fread (key, 1, DES3_KEY_SIZE, keyfh);
    fclose (keyfh);

    // read in the mac key
    keyfh = fopen (mackeyfile.c_str(), "r");
    fread (mackey, 1, DES3_KEY_SIZE, keyfh);
    fclose(keyfh);
    
    
    // read in the file contents
    string os_string;
    readfile (stdin, os_string);
    ByteBuffer orig (os_string.length());
    os_string.copy (orig.cdata(), orig.len());
    

    SymDencrypter denc ( ByteBuffer (key, DES3_KEY_SIZE, ByteBuffer::no_free) );
    MacExpert maccer ( ByteBuffer(mackey, DES3_KEY_SIZE, ByteBuffer::no_free) );
    ByteBuffer answer, mac;

    try {

	if (op == "encrypt") {
	    answer = denc.encrypt (orig);
	    cout.write (answer.cdata(), answer.len());

	    mac = maccer.genmac (answer);
	    cout.write (mac.cdata(), mac.len());
	}
	else if (op == "decrypt") {
	    // check the mac first
	    mac = ByteBuffer (orig.data() + (orig.len() - DES_MAC_SIZE),
			      DES_MAC_SIZE,
			      ByteBuffer::no_free);
	    orig.len() -= DES_MAC_SIZE;
	    
	    if ( ! maccer.checkmac (orig, mac) ) {
		cerr << "MAC check failed!!" << endl;
		return 1;
	    }
	    
	    answer = denc.decrypt (orig);
	    cout.write (answer.cdata(), answer.len());
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
    
    // aliases into o_cipher
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
// class SymDencrypter
//


// if initialized without a key, need to use the functions where a key
// is provided
SymDencrypter::SymDencrypter (SymCryptProvider & op) throw ()
    : _op  (op)
{}


SymDencrypter::SymDencrypter (const ByteBuffer& key, SymCryptProvider & op)
    : _key (key),
      _op  (op)
{}


//SymDencrypter::~SymDencrypter () {}


void
SymDencrypter::encrypt (const ByteBuffer& cleartext, ByteBuffer & o_cipher,
			const ByteBuffer& key)
{

    // use a random iv.
    byte iv[_op.IVSIZE];
    // this would take too long to do every time i think, so ditch for
    // now, and just zero it out
    // TODO: could use a counter perhaps?
//    RAND_bytes (iv, IVSIZE);
    memset (iv, 0, _op.IVSIZE);

    if (o_cipher.len() < cipherlen (cleartext.len())) {
	throw bad_arg_exception
	    ("SymDencrypter::encrypt: output buffer too small");
    }
    
    // save the IV at the start of the output buffer
    memcpy (o_cipher.data(), iv, _op.IVSIZE);

    // an alias, IVSIZE bytes into 'o_cipher', for the actual ciphertext
    ByteBuffer ciphertext (o_cipher, _op.IVSIZE, o_cipher.len() - _op.IVSIZE);
    
    // get the ciphertext
    _op.symcrypto_op (cleartext,
		      key,
		      ByteBuffer (iv, _op.IVSIZE, ByteBuffer::no_free),
		      ciphertext,
		      CRYPT_ENCRYPT);
    
    // ssl_symcrypto_op sets the length of the ciphertext
    o_cipher.len() = ciphertext.len() + _op.IVSIZE;

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
    ByteBuffer iv         (enc, 0,      _op.IVSIZE);                 // the IV
    ByteBuffer ciphertext (enc, iv.len(), enc.len() - iv.len()); // the rest

    
    if (o_clear.len() < clearlen (ciphertext.len())) {
	throw bad_arg_exception
	    ("SymDencrypter::decrypt: output buffer len "
	     + itoa (o_clear.len()) + " too small for "
	     + itoa (clearlen (ciphertext.len())) );
    }
    

    _op.symcrypto_op (ciphertext, key, iv, o_clear,
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



SymDencrypter::~SymDencrypter () {
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



#if 0
//
// class BlockDencrypter
//


void BlockDencrypter::encrypt (const ByteBuffer& in, ByteBuffer & out,
			       const ByteBuffer& key)
{
    if (out.len() < _prov.BLOCKSIZE) {
	throw length_exception;
    }

    _op.crypt_op (in, out, key, CRYPT_ENCRYPT);
}



void BlockDencrypter::decrypt (const ByteBuffer& in, ByteBuffer & out,
			       const ByteBuffer& key)
{
    if (out.len() < _prov.BLOCKSIZE) {
	throw length_exception;
    }

    _op.crypt_op (in, out, key, CRYPT_DECRYPT);
}
#endif // 0
