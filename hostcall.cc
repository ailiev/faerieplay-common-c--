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
