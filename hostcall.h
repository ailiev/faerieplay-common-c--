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



class object_id : public SERIALIZABLE(object_id, object_id_x) {

public:
    
    object_id ()
	: _levels(0)
	{
	    memset (_indices, 0, sizeof(_indices));
	}
	    

    // just one level
    object_id (index_t i)
	: _levels(1)
	{
	    _indices[0] = i;
	    _indices[1] = 0;
	}

    object_id (index_t i1, index_t i2)
	: _levels(2)
	{
	    _indices[0] = i1;
	    _indices[1] = i2;
	}
    
    object_id (const ByteBuffer& serial) {
	reconstruct(serial);
    }

    bool operator == (const object_id& b) {
	if (_levels != b._levels) return false;

	bool answer = true;
	
	switch (_levels) {
	case 3:
	    answer = answer && (_indices[2] == b._indices[2]);
	case 2:
	    answer = answer && (_indices[1] == b._indices[1]);
	case 1:
	    answer = answer && (_indices[0] == b._indices[0]);
	    break;

	default:
	    for (index_t i = 0; i < _levels; i++) {
		answer = answer && (_indices[i] == b._indices[i]);
	    }
	}

	return answer;
    }
    

    void from_xdr (const object_id_x& idx) {
	assert (idx.levels <= OID_MAXLEVELS);
	
	_levels = idx.levels;

	switch (_levels) {
	case 3:
	    _indices[2] = idx.indices[2];
	case 2:
	    _indices[1] = idx.indices[1];
	case 1:
	    _indices[0] = idx.indices[0];
	    break;

	default:
	    for (index_t i=0; i < _levels; i++) {
		_indices[i] = idx.indices[i];
	    }
	};
	    
    }
    

    void to_xdr (object_id_x & o_x) const {
	o_x.levels = _levels;

	switch (_levels) {
	case 3:			// NOTE: these *are* intended to fall through!
	    o_x.indices[2] = _indices[2];
	case 2:
	    o_x.indices[1] = _indices[1];
	case 1:
	    o_x.indices[0] = _indices[0];
	    break;

	default:
	    for (index_t i=0; i < _levels; i++) {
		o_x.indices[i] = _indices[i];
	    }
	};
    }
	

    static void free_xdr (object_id_x & x) {}


    size_t levels () const { return _levels; }
    index_t index (index_t level) const {
	assert (level < _levels);
	return _indices[level];
    }

    
protected:
    index_t _indices[OID_MAXLEVELS];
    size_t _levels;
};




struct object_name_t : public SERIALIZABLE(object_name_t,object_name_x) {

    object_name_scheme_t name_scheme;

    // can't put these in a union as both types have constructors,
    // hence just stick them both in
    object_id oid_name;
    std::string str_name;
    

    object_name_t () {}
    
    object_name_t (const object_id& id)
	: name_scheme (NAME_OBJECT_ID)
	{
	    oid_name = id;
	}

    object_name_t (const std::string& name)
	: name_scheme (NAME_STRING)
	{
	    str_name = name;
	}



    // WARNING: shallow copy of the name string here
    void to_xdr (object_name_x & o_x) const {
	o_x.name_scheme = name_scheme;

	switch (name_scheme) {
	case NAME_OBJECT_ID:
	    oid_name.to_xdr (o_x.object_name_x_u.oid_name);
	    break;
	case NAME_STRING:
	    o_x.object_name_x_u.str_name = const_cast<char*> (str_name.c_str());
	    break;
	}
    }
    
    void from_xdr (const object_name_x& namex) {
	name_scheme = namex.name_scheme;
	switch (namex.name_scheme) {
	case NAME_OBJECT_ID:
	    oid_name.from_xdr (namex.object_name_x_u.oid_name);
	    break;
	case NAME_STRING:
	    str_name = namex.object_name_x_u.str_name;
	    break;
	}
    }

    
    // nothing allocated in to_xdr(), so nothing to do here
    static void free_xdr (object_name_x & x) {}
};



#if 0
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
