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
