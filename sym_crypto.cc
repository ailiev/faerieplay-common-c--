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
    os_string.copy (reinterpret_cast<char*> (orig.data()), orig.len());
    

    SymDencrypter denc ( ByteBuffer (key, DES3_KEY_SIZE, ByteBuffer::no_free) );
    MacExpert maccer ( ByteBuffer(mackey, DES3_KEY_SIZE, ByteBuffer::no_free) );
    ByteBuffer answer, mac;

    try {

	if (op == "encrypt") {
	    answer = denc.encrypt (orig);
	    cout.write (reinterpret_cast<char*> (answer.data()), answer.len());

	    mac = maccer.genmac (answer);
	    cout.write (reinterpret_cast<char*> (mac.data()), mac.len());
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
	    cout.write (reinterpret_cast<char*> (answer.data()), answer.len());
	}
    }
    catch (crypto_exception ex) {
	cerr << "Crypto failure: " << string (ex) << endl;
    }
    
}

#endif // TESTING_SYM_CRYPTO




SymDencrypter::SymDencrypter (const ByteBuffer& key)
    : _key (key)
{

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
	 EVP_EncryptInit, EVP_EncryptUpdate, EVP_EncryptFinal,
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
	 EVP_DecryptInit, EVP_DecryptUpdate, EVP_DecryptFinal,
	 "Decryption");
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
 int (*init)   (EVP_CIPHER_CTX*, const EVP_CIPHER *, byte *, byte *),
 int (*update) (EVP_CIPHER_CTX *, byte *, int *, byte *, int ),
 int (*final)  (EVP_CIPHER_CTX *, byte *, int *),
 const string& name)
    throw (crypto_exception)
{

    if ( ! init (ctx, DES3_CBC, key.data(), NULL) ) {
	// TODO: error
	throw crypto_exception
	    ( "Initializing " + name + " context failed: " +
	      make_ssl_error_report() );
    }
    

    byte * out = new byte[input.len() + EVP_CIPHER_CTX_block_size (ctx)];
    int len = 0, running = 0;

    if ( ! update (ctx, out, &running, input.data(), input.len()) )
    {
	// TODO: error
	throw crypto_exception (name + " failed:" + make_ssl_error_report());
    }

    len += running;

    if ( ! final (ctx, out + running, &running) ) {
	// TODO: erorr
	throw crypto_exception (name+ " failed:" + make_ssl_error_report());
    }

    len += running;

    return ByteBuffer (out, len);

}
