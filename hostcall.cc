/*
 * common/hostcall.cc
 * alex iliev, jan 2003
 * routines to serialize and deserialize the communication parameters
 */


#include "comm_types.h"
#include <common/xdr_class.h>

#include "hostcall.h"


void encrypted_record::reconstruct (const ByteBuffer& serial) {

    // reconstruct 'serial' via XDR
    XDR_STRUCT(encrypted_record_x) xdr;
    xdr.decode (serial);

    bytes = xdr.x.bytes;
}


ByteBuffer encrypted_record::serialize () const {

    encrypted_record_x x = { bytes.to_xdr() };
    
    XDR_STRUCT(encrypted_record_x) xdr (x);

    return xdr.encode();
}


named_record::named_record (const ByteBuffer& serial) {

    XDR_STRUCT(named_record_x) xdr;
    xdr.decode (serial);

    id = xdr.x.id;
    rec_bytes = xdr.x.rec_bytes;
}


ByteBuffer named_record::serialize () const {

    named_record_x x;
    x.id = id;
    x.rec_bytes = rec_bytes.to_xdr();

    XDR_STRUCT(named_record_x) xdr (x);

    return xdr.encode();
}
