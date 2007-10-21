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
 * sym_crypto_hash.cc
 * alex iliev, dec 2003
 */

#include <memory>

#include <common/utils.h>

#include "sym_crypto.h"




using std::auto_ptr;


// declared in sym_crypto.h
//extern const size_t SHA1_HASHSIZE = 20; // 160 bit or 20 bytes hash size


//
// class SingleHashExpert
//

SingleHashExpert::SingleHashExpert (auto_ptr<HashProvider> prov)
    : _prov(prov)
{}


void SingleHashExpert::hash (const ByteBuffer& bytes, ByteBuffer& o_hash)
{
    _prov->singleHash (bytes, o_hash);
}


ByteBuffer SingleHashExpert::hash (const ByteBuffer& bytes)
{
    ByteBuffer answer (new byte[_prov->HASHSIZE], _prov->HASHSIZE);
    _prov->singleHash (bytes, answer);
    return answer;
}



//
// class StreamHashExpert
//

StreamHashExpert::StreamHashExpert (auto_ptr<HashProvider> prov)
    : _prov(prov)
{
    _prov->initMultiple();
}


void StreamHashExpert::addBytes (const ByteBuffer& bytes)
{
    _prov->addBytes (bytes);
}

ByteBuffer StreamHashExpert::getHash ()
{

    ByteBuffer answer (new byte[_prov->HASHSIZE], _prov->HASHSIZE);
    _prov->getHash (answer);
    _prov->initMultiple();	// ready for next batch of input bytes
    return answer;
}


void StreamHashExpert::getHash (ByteBuffer & o_hash)
{
    _prov->getHash (o_hash);
    _prov->initMultiple();
}
