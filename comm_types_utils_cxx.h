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

#include <common/countarray.hpp>

#include "comm_types.h"

#ifndef _COMM_TYPES_UTILS_CXX
#define _COMM_TYPES_UTILS_CXX

CountedByteArray from_xdr (const ByteBuffer_x & buf,
                           CountedByteArray::copy_depth_t deep = CountedByteArray::DEEP);

ByteBuffer_x to_xdr
(const ByteBuffer & buf, CountedByteArray::copy_depth_t deep = CountedByteArray::SHALLOW);

#endif  // _COMM_TYPES_UTILS_CXX
