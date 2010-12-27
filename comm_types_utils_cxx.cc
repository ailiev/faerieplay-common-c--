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

#include "comm_types_utils_cxx.h"

#include <common/countarray.hpp>

#include "comm_types.h"


/// init from an XDR representation
CountedByteArray from_xdr (const ByteBuffer_x & buf,
                           CountedByteArray::copy_depth_t deep = CountedByteArray::DEEP)
{
    if (deep != CountedByteArray::DEEP) {
        return CountedByteArray (reinterpret_cast<byte*> (buf.ByteBuffer_x_val),
                                 buf.ByteBuffer_x_len,
                                 deep);
    }
    else {
        return CountedByteArray (reinterpret_cast<byte*> (buf.ByteBuffer_x_val),
                                 buf.ByteBuffer_x_len,
                                 CountedByteArray::deepcopy());
    }
}

// WARNING: careful with the lifetime of the returned structure! This can be
// just a shallow copy
ByteBuffer_x to_xdr
(const ByteBuffer & buf, CountedByteArray::copy_depth_t deep = CountedByteArray::SHALLOW)
{

    assert (buf.len() < CountedByteArray::MAXLEN);

    ByteBuffer_x answer = { buf.len(),
                            deep == CountedByteArray::DEEP ?
                            new char[buf.len()] :
                            const_cast<char*> (buf.cdata()) };
    if (deep == CountedByteArray::DEEP)
    {
        memcpy (answer.ByteBuffer_x_val, buf.cdata(), buf.len());
    }

    return answer;
}
    
