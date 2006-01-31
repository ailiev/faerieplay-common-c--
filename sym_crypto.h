/* -*- C++ -*-
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
 * sym_crypto.h
 * alex iliev, nov 2002
 * declarations for symmetric crypto utility classes
 */

#include <string>
#include <memory>

#include <ostream>


#include <common/exceptions.h>
#include <common/utils.h>

#include "randomsrc.h"


#ifndef _SYM_CRYPTO_H
#define _SYM_CRYPTO_H



class SymCryptProvider;
class MacProvider;
class HashProvider;
class BlockCryptProvider;
class CryptoProviderFactory;


const size_t SHA1_HASHSIZE = 20; // 160 bit or 20 bytes hash size



//
// The provider base classes
//


enum crypt_op_name_t {
    CRYPT_ENCRYPT,
    CRYPT_DECRYPT
};
    
    
std::string get_crypt_op_name (crypt_op_name_t op);


class SymCryptProvider {

public:
    
    // this can only be used by derived classes of course (as this class has
    // pure virtual functions)
    SymCryptProvider (size_t ivsize, size_t blocksize, size_t keysize)
	: IVSIZE    (ivsize),
	  BLOCKSIZE (blocksize),
	  KEYSIZE   (keysize)
	{}
    
    virtual void symcrypto_op (const ByteBuffer& input,
			       const ByteBuffer& key,
			       const ByteBuffer& iv,
			       ByteBuffer & out,
			       crypt_op_name_t optype) = 0;

    virtual ~SymCryptProvider () {}

    const size_t IVSIZE, BLOCKSIZE, KEYSIZE;
    
};




class MacProvider {

public:

    MacProvider (size_t keysize, size_t macsize)
	: KEYSIZE   (keysize),
	  MACSIZE   (macsize)
	{}
    
    virtual void genmac (const ByteBuffer& text, const ByteBuffer& key,
			 ByteBuffer & out) = 0;

    virtual ~MacProvider() {}

    const size_t KEYSIZE, MACSIZE;
};





class SymDencrypter {



public:

    SymDencrypter (std::auto_ptr<SymCryptProvider>  op,
		   std::auto_ptr<RandProvider>	    rand)
	throw ();
    
    SymDencrypter (std::auto_ptr<SymCryptProvider>  op,
		   std::auto_ptr<RandProvider>	    rand,
		   const ByteBuffer& 		    key)
	throw ();

    
    // dencrypt into a provided buffer
    // set the outpur buffer's final length
    void encrypt (const ByteBuffer& cleartext, ByteBuffer & o_cipher)
	{
	    encrypt (cleartext, o_cipher, _key);
	}

    void decrypt (const ByteBuffer& ciphertext, ByteBuffer & o_clear)
	{
	    decrypt (ciphertext, o_clear, _key);
	}

    // as above, but provide the key for this operation
    void encrypt (const ByteBuffer& cleartext, ByteBuffer & o_cipher,
		  const ByteBuffer& key);
    
    void decrypt (const ByteBuffer& ciphertext, ByteBuffer & o_clear,
		  const ByteBuffer& key);


    void setkey (const ByteBuffer& key) throw ();
    ByteBuffer getkey () throw ();
    void genkey () throw (crypto_exception);
    
    //
    // some size hints
    //
    size_t cipherlen (size_t clearlen) {
// 	std::clog << "SymDencrypter::cipherlen: "
// 		  << "_op->IVSIZE = " << _op->IVSIZE
// 		  << "_op->BLOCKSIZE = " << _op->BLOCKSIZE
// 		  << std::endl;
	return clearlen + _op->IVSIZE + _op->BLOCKSIZE;
    }
    
    size_t clearlen (size_t cipherlen) {
	// size requierements from the openSSL docs EVP_DecryptInit_ex(3ssl)
	// seems a bit mysterious why we may need up to an extra block for the
	// cleartext!
	return cipherlen - _op->IVSIZE + _op->BLOCKSIZE;
    }

    // return the IV length
    size_t ivsize () { return _op->IVSIZE; }

    size_t keylen () { return _op->KEYSIZE; }
    
    
    // convenience functions which allocate the output buf internally
    ByteBuffer
    encrypt (const ByteBuffer& cleartext);

    ByteBuffer
    decrypt (const ByteBuffer& ciphertext);

    ~SymDencrypter();


private:
    
    ByteBuffer _key;
    std::auto_ptr<SymCryptProvider> _op;
    std::auto_ptr<RandProvider>	    _rand;
};




class MacExpert {

public:
    
    MacExpert (std::auto_ptr<MacProvider> op,
	       std::auto_ptr<RandProvider> rand,
	       const ByteBuffer& key);

    // setup the key later
    MacExpert (std::auto_ptr<MacProvider> op,
	       std::auto_ptr<RandProvider> rand);

    // generate mac into the given space
    void
    genmac (const ByteBuffer& text, ByteBuffer & o_mac);


    // allocate the mac buffer internally
    ByteBuffer
    genmac (const ByteBuffer& text);
    

    bool
    checkmac(const ByteBuffer& text, const ByteBuffer& mac);

    void setkey (const ByteBuffer& key) throw ();
    ByteBuffer getkey ()  throw ();

    void genkey () throw (crypto_exception);

    // assume it's independent of text size
    size_t maclen() {
	return _op->MACSIZE;
    }

private:

    ByteBuffer 			_key;

    std::auto_ptr<MacProvider>	_op;
    std::auto_ptr<RandProvider>	_rand;

};




//
// provides methods to encrypt and add integrity data to a byte buffer
// fairly thin wrapper around SymDencrypter and MacExpert, to provide the
// structure of a macc-ed ciphertext
//
class SymWrapper {

public:

//     SymWrapper (const ByteBuffer& key, std::auto_ptr<SymCryptProvider> sym_prov,
// 		const ByteBuffer& mackey, std::auto_ptr<MacProvider> mac_prov)
//         : _denc   (key, sym_prov),
//           _maccer (mackey, mac_prov),
//           _do_mac (true)
// 	{}

    SymWrapper (const ByteBuffer& key,
		const ByteBuffer& mackey,
		CryptoProviderFactory * provfact)
        throw (crypto_exception);

    // generate keys here
    SymWrapper (CryptoProviderFactory * provfact)
        throw (crypto_exception);


    // wrap/unwrap into a provided buffer
    void wrap (const ByteBuffer& cleartext, ByteBuffer & o_cipher);

    void unwrap (const ByteBuffer& ciphertext, ByteBuffer & o_clear);
    

    // methods to say how much will be needed for the return buffer
    size_t wraplen (size_t clearlen) {
	return _denc.cipherlen (clearlen) + _maccer.maclen();
    }
    
    size_t unwraplen (size_t cipherlen) {
	return _denc.clearlen ( cipherlen - _maccer.maclen() );
    }
    

    // convenience methods which allocate the return buffer internally
    ByteBuffer wrap (const ByteBuffer& cleartext);
    
    ByteBuffer unwrap (const ByteBuffer& ciphertext);

    // and key manipulation routines
    void setkey (const ByteBuffer& key) throw ();
    void setmackey (const ByteBuffer& key) throw ();

    ByteBuffer getkey ()    throw ();
    ByteBuffer getmackey () throw ();
    void genkeys ()	    throw (crypto_exception);
    

private:
    SymDencrypter _denc;
    MacExpert     _maccer;

    bool          _do_mac;
};






//
// and the interface to a hash provider
//
class HashProvider {

public:

    HashProvider (size_t hashsize) : HASHSIZE (hashsize) {}
    
    // for producing just a single hash, leaving the object in an initial state
    // again
    virtual void singleHash (const ByteBuffer& bytes, ByteBuffer & o_hash) = 0;

    // and these are for generating a hash in stages
    virtual void initMultiple() = 0;
    virtual void addBytes(const ByteBuffer& bytes) = 0;
    // getHash() leaves the object in an initial state
    virtual void getHash(ByteBuffer & o_hash) = 0;


    virtual ~HashProvider() {}
    

    const size_t HASHSIZE;

};


    



//
// class to generate cryptographic hashes for a single buffer at a time
//


class SingleHashExpert {

public:

    SingleHashExpert (std::auto_ptr<HashProvider> provider);

    //
    // two versions of 'hash' for convenience
    //
    
    // WARNING: o_hash has to be sufficiently allocated, and the hash
    // will be written straight into its buffer, so any aliases to
    // o_hash will also be overwritten!
    void hash (const ByteBuffer& bytes, ByteBuffer & o_hash);

    ByteBuffer hash (const ByteBuffer& bytes);

    size_t hashSize () const { return _prov->HASHSIZE; }


private:
    std::auto_ptr<HashProvider> _prov;

};



//
// class for hashing a buffer in pieces
//
class StreamHashExpert {

public:

    StreamHashExpert (std::auto_ptr<HashProvider> prov);

    void addBytes (const ByteBuffer& bytes);

    // this can be called only once, and after that the object is ready for
    // another batch of addBytes()
    ByteBuffer getHash();

    // and with caller-provider buffer
    void getHash (ByteBuffer & o_hash);

    size_t hashSize() const { return _prov->HASHSIZE; }

    
private:
    std::auto_ptr<HashProvider> _prov;
    
};



//
//
// a class to do single-block enc/decryptions, should be a lot faster
// than CBC mode multiple blocks 
//
//

// first its virtual provider class
class BlockCryptProvider {

public:

    BlockCryptProvider (size_t keysize, size_t blocksize)
	: KEYSIZE   (keysize),
	  BLOCKSIZE (blocksize)
	{}

    
    virtual void setkey (const ByteBuffer& key)
	throw (crypto_exception)		    = 0;
    
    virtual void crypt_op (const ByteBuffer& in,
			   ByteBuffer & out,
			   crypt_op_name_t op)
	throw (crypto_exception)		    = 0;

    virtual ~BlockCryptProvider () {}

    const size_t KEYSIZE, BLOCKSIZE;
    
};




class BlockDencrypter {

public:

    BlockDencrypter (std::auto_ptr<BlockCryptProvider> prov)
	throw (crypto_exception);
    
    void setkey (const ByteBuffer& key)
	throw (crypto_exception);
    
    void encrypt (const ByteBuffer& in, ByteBuffer & out)
	throw (crypto_exception);
    
    void decrypt (const ByteBuffer& in, ByteBuffer & out)
	throw (crypto_exception);

    size_t keylen ()
	{ return _prov->KEYSIZE; }
    size_t blocklen ()
	{ return _prov->BLOCKSIZE; }

private:

    std::auto_ptr<BlockCryptProvider> _prov;
};





class CryptoProviderFactory {

public:
    
    virtual std::auto_ptr<SymCryptProvider> getSymCryptProvider()
	throw (crypto_exception)                                = 0;
    virtual std::auto_ptr<MacProvider>      getMacProvider()
	throw (crypto_exception)                                = 0;
    virtual std::auto_ptr<HashProvider>     getHashProvider()
	throw (crypto_exception)                                = 0;
    virtual std::auto_ptr<RandProvider>     getRandProvider ()
        throw (crypto_exception)                                = 0;
    virtual std::auto_ptr<BlockCryptProvider> getBlockCrypt ()
	throw (crypto_exception)				= 0;

    virtual size_t keysize ()                                   = 0;
    virtual size_t mac_keysize ()                               = 0;

    virtual ~CryptoProviderFactory () {}

};

    
//
// some helpers
//

SymWrapper mkRandWrapper (CryptoProviderFactory * fact)
    throw (crypto_exception);




//
// all the following are in bytes
//

// defined in sym_crypto.cc
extern const size_t DES_MAC_SIZE;
extern const size_t DES3_KEY_SIZE;

// in sym_crypto_mac.cc
extern const size_t HMAC_SHA1_KEYSIZE;
extern const size_t HMAC_SHA1_MACSIZE;

// in sym_crypto_hash.cc
extern const size_t SHA1_HASHSIZE;





#endif // _SYM_CRYPTO_H
