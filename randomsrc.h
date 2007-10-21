// -*- c++ -*-
/*
 * Library routines for the Dartmouth CS Private Information Retrieval and
 * Writing (PIR/W) library.
 *
 * Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu> and
 * Sean W. Smith <sws@cs.dartmouth.edu>
 *
 * All rights reserved.
 *
 * This code is released under a BSD license.
 * Please see LICENSE.txt for the full license and disclaimers.
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com)
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
