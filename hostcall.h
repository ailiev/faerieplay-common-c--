/* -*- c++ -*-
 * hostcall.h
 * alex iliev, nov 2002
 * headers of host service invocation interface
 */

#ifndef _HOSTCALL_H
#define _HOSTCALL_H


#include <common/utils.h>
#include <common/record.h>




class HostParams;
class HostResult;


//
// the possible status codes from a host call
//
enum host_status_t {

    STATUS_OK = 0,
    STATUS_IO_ERROR,
    STATUS_DIR_ERROR,

};



//
// 
//



//
// some host call structures
//

//
// fetch an encrypted record from the host
//

struct record_id {
    record_id () {}
    
    record_id (index_t bucket, index_t record) :
	bucket(bucket),
	record (record) {}
    
    index_t bucket;
    index_t record;	// record number in this bucket
};


struct encrypted_record {
    encrypted_record () {}
    
    encrypted_record (ByteBuffer bytes) : bytes (bytes) {}

    
    ByteBuffer bytes;		// the ciphertext + MAC
};
    


//
// write an encrypted record
//
struct named_record {

    named_record (index_t bucket, index_t record, const ByteBuffer& bytes)
	: id (bucket, record),
	  rec_bytes (bytes)
	{}
		      
    record_id id;
    ByteBuffer rec_bytes;
};



//
//
// the implementing host functions
// defined in host/hostcall.cc
//
//

host_status_t host_fetch_record (const record_id* id,
				 encrypted_record* o_result);

host_status_t host_fetch_clearrecord (const record_id*,
				      Record * o_result);

host_status_t host_write_record (const named_record*);



#endif // _HOSTCALL_H
