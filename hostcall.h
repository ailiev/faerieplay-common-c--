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
