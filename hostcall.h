/* -*- c++ -*-
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
 * hostcall.h
 * alex iliev, nov 2002
 * headers of host service invocation interface
 */


#ifndef _COMMON_HOSTCALL_H
#define _COMMON_HOSTCALL_H


#include <assert.h>

#include <common/utils.h>
#include <common/record.h>
#include <common/xdr_class.h>
#include <common/exceptions.h>

#include "comm_types.h"




//
// some host call structures
//


#if 0

#define OIDLEVELS 2

class object_id {

public:
    
    object_id () {}

    object_id (index_t indices_[OIDLEVELS])
	{
	    std::copy (indices_, indices_ + OIDLEVELS, _indices);
	}
    
    object_id (const ByteBuffer& serial) {
	reconstruct(serial);
    }

    object_id & operator= (index_t indices_[OIDLEVELS]) {
	std::copy (indices_, indices_ + OIDLEVELS, _indices);
	return *this;
    }


    
    void from_xdr (const object_id_x& idx) {
	assert (idx.object_id_x_len == OIDLEVELS);
	std::copy (idx.object_id_x_val,
		   idx.object_id_x_val + idx.object_id_x_len,
		   _indices);
    }
    
    void reconstruct (const ByteBuffer& serial) throw (xdr_exception) {
	XDR_STRUCT(object_id_x) xdr;
	xdr.decode (serial);
	from_xdr (xdr.x);
    }

    // note: shallow copy!
    void to_xdr (object_id_x & o_x) const {
	o_x.object_id_x_len = OIDLEVELS;
	o_x.object_id_x_val = _indices;
    }
	
    ByteBuffer serialize () const throw (xdr_exception) {
	object_id_x id_x;
	to_xdr (id_x);
	return ENCODE_XDR(object_id_x) (id_x);
    }
	

private:
    index_t _indices[OIDLEVELS];
};


struct object_name_t {

    object_name_scheme_t name_scheme;
    union name_u {
	object_id oid;
	std::string str;
    };


    object_name_t (const object_id& id)
	: name_scheme (NAME_OBJECT_ID)
	{
	    name_u.oid = id;
	}

    object_name_t (const std::string& name)
	: name_scheme (NAME_STRING)
	{
	    name_u.str = name;
	}



    void to_xdr (object_name_x & o_x) const {
	o_x.name_scheme = name_scheme;

	switch (name_scheme) {
	case NAME_OBJECT_ID:
	    name_u.oid.to_xdr (o_x.object_name_t_u.oid_name);
	    break;
	case NAME_STRING:
	    o_x.object_name_t_u.str_name = name_u.str.c_str();
	    break;
	}
    }
    
    ByteBuffer serialize () const throw (xdr_exception) {
	object_name_x namex;
	to_xdr (namex);
	return ENCODE_XDR(object_name_x) (namex);
    }

    void from_xdr (const object_name_x& namex) {
	name_scheme = namex.name_scheme;
	switch (namex.name_scheme) {
	case NAME_OBJECT_ID:
	    name_u.oid.from_xdr (namex.object_name_t_u.oid_name);
	    break;
	case NAME_STRING:
	    name_u.str = namex.object_name_t_u.str_name;
	    break;
	}
    }
    
    void reconstruct (const ByteBuffer& serial) throw (xdr_exception) {
	XDR_STRUCT(object_name_x) xdr;
	xdr.decode(serial);
	from_xdr (xdr.x);
    }
	
};


#endif


struct blob {
    blob () {}
    
    blob (const ByteBuffer& bytes) : bytes (bytes) {}

    void reconstruct (const ByteBuffer& serial);
    ByteBuffer serialize () const;
    

    ByteBuffer bytes;		// the ciphertext + MAC
};
    


//
// write an encrypted record
//
#if 0
struct named_blob {

    named_blob (const object_id& id, const ByteBuffer& bytes)
	: id (id),
	  rec_bytes (bytes)
	{}

    named_blob (const ByteBuffer& serial);

    ByteBuffer serialize () const;
    

    object_id id;
    ByteBuffer rec_bytes;
};
#endif // 0




#endif // _COMMON_HOSTCALL_H
