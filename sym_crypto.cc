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
    : _key (key)
{

    if (EVP_CIPHER_CTX_init (&_context) == 0) {
	throw crypto_exception ( "Initializing context failed: " +
				 make_ssl_error_report() );
    }
    
    // need to:
    // - set the key

}


//SymDencrypter::~SymDencrypter () {}


ByteBuffer
SymDencrypter::encrypt (const ByteBuffer& cleartext)
    throw (crypto_exception)
{
    // need to:
    // - maybe initilaize the context for encryption
    // - call the EVP update func
    // - catch its output and put into our buffer


    return ssl_symcrypto_op
	(cleartext, _key, &_context,
	 EVP_EncryptInit_ex, EVP_EncryptUpdate, EVP_EncryptFinal_ex,
	 "Encryption");
    
}




ByteBuffer
SymDencrypter::decrypt (const ByteBuffer& ciphertext)
    throw (crypto_exception)
{
    // need to:
    // - maybe initilaize the context for decryption
    // - call the EVP update func
    // - catch its output and put into our buffer


    return ssl_symcrypto_op
	(ciphertext, _key, &_context,
	 EVP_DecryptInit_ex, EVP_DecryptUpdate, EVP_DecryptFinal_ex,
	 "Decryption");
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
	





ByteBuffer
ssl_symcrypto_op
(const ByteBuffer& input,
 const ByteBuffer& key,
 EVP_CIPHER_CTX *ctx,
 init_func_t init, update_func_t update, final_func_t final,
 const string& name)
    throw (crypto_exception)
{
    const EVP_CIPHER * cipher = DES3_CBC;
    const size_t BLOCKSIZE = EVP_CIPHER_block_size (cipher);
    
    // use a random iv. just prepate the largest one used by OpenSSL here
    byte iv[EVP_MAX_IV_LENGTH];
    RAND_bytes (iv, EVP_MAX_IV_LENGTH);
    
    if ( ! init (ctx, cipher, NULL, key.data(), iv) ) {
	throw crypto_exception
	    ( "Initializing " + name + " context failed: " +
	      make_ssl_error_report() );
    }
    

    // according to the SSL docs, the output will be at most one block larger
    // than the input
    ByteBuffer answer (new byte[input.len() + BLOCKSIZE], 0);
    int running = 0;

    if ( ! update (ctx, answer.data(), &running, input.data(), input.len()) ) {
	throw crypto_exception (name + " failed:" + make_ssl_error_report());
    }

    answer.len() += running;

    if ( ! final (ctx, answer.data() + running, &running) ) {
	throw crypto_exception (name+ " failed:" + make_ssl_error_report());
    }

    answer.len() += running;

    return answer;

}
