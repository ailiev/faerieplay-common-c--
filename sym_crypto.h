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


#include <common/exceptions.h>
#include <common/utils.h>

//#include "cbcmac.h"


#ifndef _SYM_CRYPTO_H
#define _SYM_CRYPTO_H



class SymCryptProvider;
class MacProvider;
class HashProvider;
class BlockCryptProvider;




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
    SymCryptProvider (size_t IVSIZE, size_t BLOCKSIZE)
	: IVSIZE    (IVSIZE),
	  BLOCKSIZE (BLOCKSIZE)
	{}
    
    virtual void symcrypto_op (const ByteBuffer& input,
			       const ByteBuffer& key,
			       const ByteBuffer& iv,
			       ByteBuffer & out,
			       crypt_op_name_t optype)
	throw (crypto_exception) = 0;

    virtual ~SymCryptProvider () {}

    const size_t IVSIZE, BLOCKSIZE;
    
};




class MacProvider {

public:

    MacProvider (size_t MACSIZE) throw (crypto_exception)
	: MACSIZE (MACSIZE)
	{}
    
    virtual void genmac (const ByteBuffer& text, const ByteBuffer& key,
			 ByteBuffer & out)
	throw (crypto_exception) = 0;

    virtual ~MacProvider() {}

    const size_t MACSIZE;
};





class SymDencrypter {



public:

    SymDencrypter (SymCryptProvider & op) throw ();
    
    SymDencrypter (const ByteBuffer& key, SymCryptProvider & op)
	throw (crypto_exception);

    
    // dencrypt into a provided buffer
    // set the outpur buffer's final length
    void encrypt (const ByteBuffer& cleartext, ByteBuffer & o_cipher)
	throw (crypto_exception)
	{
	    encrypt (cleartext, o_cipher, _key);
	}

    void decrypt (const ByteBuffer& ciphertext, ByteBuffer & o_clear)
	throw (crypto_exception)
	{
	    decrypt (ciphertext, o_clear, _key);
	}

    // as above, but provide the key for this operation
    void encrypt (const ByteBuffer& cleartext, ByteBuffer & o_cipher,
		  const ByteBuffer& key)
	throw (crypto_exception);
    
    void decrypt (const ByteBuffer& ciphertext, ByteBuffer & o_clear,
		  const ByteBuffer& key)
	throw (crypto_exception);


    // some size hints
    size_t cipherlen (size_t clearlen) {
	return clearlen + _op.IVSIZE + _op.BLOCKSIZE;
    }
    
    size_t clearlen (size_t cipherlen) {
	// size requierements from the openSSL docs EVP_DecryptInit_ex(3ssl)
	// seems a bit mysterious why we may need up to an extra block for the
	// cleartext!
	return cipherlen - _op.IVSIZE + _op.BLOCKSIZE;
    }

    // return the IV length
    size_t ivsize () { return _op.IVSIZE; }
    
    
    // convenience functions which allocate the output buf internally
    ByteBuffer
    encrypt (const ByteBuffer& cleartext)
	throw (crypto_exception);

    ByteBuffer
    decrypt (const ByteBuffer& ciphertext)
	throw (crypto_exception);

    ~SymDencrypter();


private:
    
    ByteBuffer _key;
    SymCryptProvider & _op;
};




class MacExpert {

public:
    
    MacExpert (const ByteBuffer& key, MacProvider & op);

    // generate mac into the given space
    void
    genmac (const ByteBuffer& text, ByteBuffer & o_mac)
	throw (crypto_exception);


    // allocate the mac buffer internally
    ByteBuffer
    genmac (const ByteBuffer& text) throw (crypto_exception);
    

    bool
    checkmac(const ByteBuffer& text, const ByteBuffer& mac)
	throw (crypto_exception);

    // assume it's independent of text size
    size_t maclen() {
	return _op.MACSIZE;
    }



private:

    const ByteBuffer _key;
    MacProvider &    _op;

};




//
// provides methods to encrypt and add integrity data to a byte buffer
// fairly thin wrapper around SymDencrypter and MacExpert, to provide the
// structure of a macc-ed ciphertext
//
class SymWrapper {

public:

    // this version will do MAC's
    SymWrapper (const ByteBuffer& key, SymCryptProvider & sym_prov,
		const ByteBuffer& mackey, MacProvider & mac_prov) :
	_denc   (key, sym_prov),
	_maccer (mackey, mac_prov),
	_do_mac (true)
	{}


    // wrap/unwrap into a provided buffer
    void wrap (const ByteBuffer& cleartext, ByteBuffer & o_cipher)
	throw (crypto_exception);

    void unwrap (const ByteBuffer& ciphertext, ByteBuffer & o_clear)
	throw (crypto_exception);
    

    // methods to say how much will be needed for the return buffer
    size_t wraplen (size_t clearlen) {
	return _denc.cipherlen (clearlen) + _maccer.maclen();
    }
    
    size_t unwraplen (size_t cipherlen) {
	return _denc.clearlen ( cipherlen - _maccer.maclen() );
    }
    

    // convenience methods which allocate the return buffer internally
    ByteBuffer wrap (const ByteBuffer& cleartext)
	throw (crypto_exception);
    
    ByteBuffer unwrap (const ByteBuffer& ciphertext)
	throw (crypto_exception);
    

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
    virtual void singleHash (const ByteBuffer& bytes, ByteBuffer & o_hash)
	throw (crypto_exception) = 0;

    // and these are for generating a hash in stages
    virtual void initMultiple() = 0;
    virtual void addBytes(const ByteBuffer& bytes)
	throw (crypto_exception) = 0;
    // getHash() leaves the object in an initial state
    virtual void getHash(ByteBuffer & o_hash) throw (crypto_exception) = 0;


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
    void hash (const ByteBuffer& bytes, ByteBuffer & o_hash)
	throw (crypto_exception);

    ByteBuffer hash (const ByteBuffer& bytes) throw (crypto_exception);

    size_t hashSize () const { return _prov->HASHSIZE; }


private:
    std::auto_ptr<HashProvider> _prov;

};



//
// class for hashing a buffer in pieces
//
class StreamHashExpert {

public:

    StreamHashExpert (std::auto_ptr<HashProvider> prov)
	throw (crypto_exception);

    void addBytes (const ByteBuffer& bytes) throw (crypto_exception);

    // this can be called only once, and after that the object is ready for
    // another batch of addBytes()
    ByteBuffer getHash() throw (crypto_exception);

    // and with caller-provider buffer
    void getHash (ByteBuffer & o_hash) throw (crypto_exception);

    size_t hashSize() const { return _prov->HASHSIZE; }

    
private:
    std::auto_ptr<HashProvider> _prov;
    
};



#if BLOCKDENC
//
// a class to do single-block enc/decryptions, should be a lot faster
// than CBC mode multiple blocks 
//

class BlockDencrypter {

public:

    BlockDencrypter (BlockCryptProvider & prov)
	: _prov (prov)
	{}
    
    void encrypt (const ByteBuffer& in, ByteBuffer & out,
		  const ByteBuffer& key)
	throw (crypto_exception);
    
    void decrypt (const ByteBuffer& in, ByteBuffer & out,
		  const ByteBuffer& key)
	throw (crypto_exception);

private:

    BlockCryptProvider & _prov;
};




class BlockCryptProvider {

public:

    BlockCryptProvider (size_t blocksize)
	: BLOCKSIZE (blocksize)
	{}
    
    virtual void crypt_op (const ByteBuffer& in,
			   ByteBuffer & out,
			   const ByteBuffer& key,
			   crypt_op_name_t op)
	throw (crypto_exception);

    virtual ~BlockCryptProvider () {}

    const size_t BLOCKSIZE;
    
};

#endif // BLOCKDENC



class CryptoProviderFactory {

public:
    
    virtual std::auto_ptr<SymCryptProvider> getSymCryptProvider()
	throw (crypto_exception)                             = 0;
    virtual std::auto_ptr<MacProvider>      getMacProvider()
	throw (crypto_exception)                             = 0;
    virtual std::auto_ptr<HashProvider>     getHashProvider()
	throw (crypto_exception)                             = 0;

    virtual ~CryptoProviderFactory () {}

};

    



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
