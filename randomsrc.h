// -*- c++ -*-
/*
Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu>

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of the author nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

This product includes cryptographic software written by Eric Young
(eay@cryptsoft.com)

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <common/utils.h>
#include <common/exceptions.h>

#ifndef _RANDOMSRC_H
#define _RANDOMSRC_H


class RandProvider {

public:
    
    // fills in an already-allocated ByteBuffer to capacity
    virtual void randbytes (ByteBuffer & out) throw (crypto_exception)
        = 0;

    // and for convenience
    ByteBuffer alloc_randbytes (size_t howmany) throw (crypto_exception)
        {
            ByteBuffer answer (howmany);
            randbytes (answer);
            return answer;
        }

    int randint (int max) throw (crypto_exception)
	{
	    ByteBuffer randbytes = alloc_randbytes (divup (lgN_ceil(max), 8));
	    int answer = 0;

	    assert (randbytes.len() <= sizeof(answer));
	    
	    // FIXME: this will not work with big-endian int's!
	    memcpy (&answer, randbytes.data(), randbytes.len());

	    return answer % max;
	}
	    

    virtual ~RandProvider ()
	{}

};


#endif // _RANDOMSRC_H
