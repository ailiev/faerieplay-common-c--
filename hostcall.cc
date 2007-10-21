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
 * common/hostcall.cc
 * alex iliev, jan 2003
 * routines to serialize and deserialize the communication parameters
 */


#include "comm_types.h"
#include <common/xdr_class.h>

#include "hostcall.h"



#if 0
void blob::reconstruct (const ByteBuffer& serial) {

    // reconstruct from 'serial' via XDR
    XDR_STRUCT(blob_x) xdr;
    xdr.decode (serial);

    bytes = xdr.x.bytes;
}


ByteBuffer blob::serialize () const {

    blob_x x = { bytes.to_xdr() };
    
    XDR_STRUCT(blob_x) xdr (x);

    return xdr.encode();
}


named_blob::named_blob (const ByteBuffer& serial) {

    XDR_STRUCT(named_blob_x) xdr;
    xdr.decode (serial);

    id = xdr.x.id;
    rec_bytes = xdr.x.rec_bytes;
}


ByteBuffer named_blob::serialize () const {

    named_blob_x x;
    x.id = id;
    x.rec_bytes = rec_bytes.to_xdr();

    XDR_STRUCT(named_blob_x) xdr (x);

    return xdr.encode();
}
#endif // 0


