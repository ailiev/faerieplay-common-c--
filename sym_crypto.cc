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


#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include <common/utils.h>

#include <sym_crypto.h>


using namespace std;



const size_t DES_MAC_SIZE = 8;
const size_t DES3_KEY_SIZE = 3*8;

const size_t DES_BLOCK_SIZE = 8; // bytes

const EVP_CIPHER * DES3_CBC = EVP_des_ede3_cbc();
const EVP_CIPHER * DES3_ECB = EVP_des_ede3();




#ifdef TESTING_SYM_CRYPTO

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
    ByteBuffer orig (new byte[os_string.length()], os_string.length());
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




SymDencrypter::SymDencrypter (const ByteBuffer& key) throw (crypto_exception)
    : _cipher (DES3_CBC),
      _key    (key)
{

    EVP_CIPHER_CTX_init (&_context);
    
    // need to:
    // - set the key

}


//SymDencrypter::~SymDencrypter () {}


ByteBuffer
SymDencrypter::encrypt (const ByteBuffer& cleartext)
    throw (crypto_exception)
{

    const size_t IVSIZE =    EVP_CIPHER_iv_length  (_cipher);
    const size_t BLOCKSIZE = EVP_CIPHER_block_size (_cipher);
	
    // use a random iv.
    byte iv[IVSIZE];
    RAND_bytes (iv, IVSIZE);

    // enough size for the IV, ciphertext and any padding
    size_t outlen = cleartext.len() + IVSIZE + BLOCKSIZE;
    ByteBuffer answer (new byte [outlen], outlen);
    
    // save the IV
    memcpy (answer.data(), iv, IVSIZE);

    // an alias, IVSIZE bytes into 'answer', for the actual ciphertext
    ByteBuffer ciphertext (answer, IVSIZE, answer.len() - IVSIZE);
    
    // get the ciphertext
    ssl_symcrypto_op (cleartext,
		      ByteBuffer (iv, IVSIZE, ByteBuffer::no_free),
		      ciphertext,
		      ENCRYPT);
    
    // ssl_symcrypto_op sets the length of the ciphertext
    answer.len() = ciphertext.len() + IVSIZE;

    return answer;
    
}




ByteBuffer
SymDencrypter::decrypt (const ByteBuffer& enc)
    throw (crypto_exception)
{
    // need to:
    // retrieve the IV from front of buffer
    // call the generic function

    const size_t IVSIZE =    EVP_CIPHER_iv_length  (_cipher);
    const size_t BLOCKSIZE = EVP_CIPHER_block_size (_cipher);
    
    ByteBuffer iv         (enc, 0,      IVSIZE);             // the IV
    ByteBuffer ciphertext (enc, IVSIZE, enc.len() - IVSIZE); // the rest

    // size requierements from the openSSL docs
    size_t outlen = ciphertext.len() + BLOCKSIZE;
    ByteBuffer cleartext (new byte[outlen], outlen);

    ssl_symcrypto_op (ciphertext, iv, cleartext, DECRYPT);

    // the length of 'cleartext' will have been set by ssl_symcrypto_op()

    return cleartext;
}



SymDencrypter::~SymDencrypter () {
    EVP_CIPHER_CTX_cleanup(&_context);
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
	





void SymDencrypter::ssl_symcrypto_op (const ByteBuffer& input,
				      const ByteBuffer& iv,
				      ByteBuffer & out,
				      OpType optype)
    throw (crypto_exception)
{
    
    string name = optype == ENCRYPT ? "Encrypt" : "Decrypt";

    // will prepend the IV to the ciphertext, and then fetch it from there at
    // decryption time
    if ( ! EVP_CipherInit_ex (&_context, _cipher, NULL, _key.data(), iv.data(),
			      optype == ENCRYPT ? 1 : 0) )
    {
	throw crypto_exception
	    ( "Initializing " + name + " context failed: " +
	      make_ssl_error_report() );
    }
    

    byte * inbuf = input.data();
    int inlen = input.len();
    
    int outlen = 0;
    size_t running = 0;
    
    
    if ( ! EVP_CipherUpdate (&_context, out.data(), &outlen, inbuf, inlen) )
    {
	throw crypto_exception (name + " failed:" + make_ssl_error_report());
    }

    running += outlen;

    if ( ! EVP_CipherFinal_ex (&_context, out.data() + running, &outlen) ) {
	throw crypto_exception (name + " failed:" + make_ssl_error_report());
    }

    running += outlen;

    out.len() = running;
}
