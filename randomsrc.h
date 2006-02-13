// -*- c++ -*-

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
