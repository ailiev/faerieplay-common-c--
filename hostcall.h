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

/* -*- c++ -*-
 * hostcall.h
 * alex iliev, nov 2002
 * headers of host service invocation interface
 */

#ifndef _HOSTCALL_H
#define _HOSTCALL_H


#include <common/utils.h>
#include <common/record.h>

#include "comm_types.h"



//
// some host call structures
//

// record_id is in comm_types.h


struct encrypted_record {
    encrypted_record () {}
    
    encrypted_record (const ByteBuffer& bytes) : bytes (bytes) {}

    void reconstruct (const ByteBuffer& serial);
    ByteBuffer serialize () const;
    

    ByteBuffer bytes;		// the ciphertext + MAC
};
    


//
// write an encrypted record
//
struct named_record {

    named_record (const record_id& id, const ByteBuffer& bytes)
	: id (id),
	  rec_bytes (bytes)
	{}

    named_record (const ByteBuffer& serial);

    ByteBuffer serialize () const;
    

    record_id id;
    ByteBuffer rec_bytes;
};


#endif // _HOSTCALL_H
