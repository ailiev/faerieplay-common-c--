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

// xdr_class.h
// alex iliev, nov 2002
// class to wrap an XDR-generated struct

// The interface is a bit funny:
// for encoding:
// XDR_STRUCT(T) xdr (val);
// ByteBuffer enc = xdr.encode();
//
// decoding:
// XDR_STRUCT(T) xdr ();
// xdr.decode (enc);
// the decoded value (of type T) is in xdr.x

// the XDRStruct is only useful because a decoded value needs to be
// freed (with the XDR_FREE filter operation) when done with it, so
// this is done in the XDRStruct destructor.
// 
// encoding would be better done from a standalone function, as there
// is no such cleanup needed there. Have such a function at the end: encode_xdr



#ifndef _XDRSTRUCT_H
#define _XDRSTRUCT_H


#include <errno.h>

#include <iostream>

#include <rpc/xdr.h>

#include <common/utils.h>
#include <common/consts.h>
#include <common/exceptions.h>


// helper struct and function for using the xdrrec_create XDR stream
// type
// 'pos' is the index where the next written byte goes, or where the
// next byte is to be read
struct quick_buf {
    size_t pos, capacity;
    byte * data;
};


ssize_t write_xdrrec (char * handle, char *data, int count);

ssize_t read_xdrrec (char * handle, char * buf, int count);


// wrapper around the templated XDRStruct constructor, to avoid typing the
// typename and filter function twice
#define XDR_STRUCT(T) XDRStruct<T, xdr_ ## T>


// the T is the xdr-generated struct, and Filter is the XDR filter
// function
//
// eg. if T is Record, Filter will probably be xdr_Record
template <class T, bool_t (*Filter) (XDR*, T*) >
class XDRStruct {

public:
    // the actual struct, free for all to set/modify
    T x;


    XDRStruct ()
	: should_free_struct (false) {}

    XDRStruct (const T& x)
	: x (x),
	  should_free_struct (false) {}
    
    //
    // NOTE: using the xdrrec stream type results in a 7-8%
    // performance hit for the shuffling part, which is quite a bit.
    // so, try again with the xdrmem, and reallocating upon memory
    // failure.

#ifdef _XDR_USE_XDRREC_STREAM
    ByteBuffer encode () const throw (xdr_exception) {

	//               pos cap  data
	quick_buf buf = { 0, 1024, new byte [1024] };
	if (!buf.data) {
	    throw xdr_exception
		("Encoding failed to allocate memory for quick_buf", errno);
	}
	
	xdrrec_create (&xdr,
		       BUFSIZE, 1, // don't need a read buffer, so make
		                   // it 1 byte
		       (char*) &buf,
		       NULL, write_xdrrec);
       
	xdr.x_op = XDR_ENCODE;
       
	if ( ! Filter (&xdr, const_cast<T*> (&x)) ) {
	    throw xdr_exception (std::string("Encoding ") + typeid(T).name(),
				 errno);
	}

	// tell it we're done writing and it should flush its buffer
	// to ours
	if ( xdrrec_endofrecord (&xdr, 1) == 0 ) {
	    throw xdr_exception
		("Flushing XDR stream failed", errno);
	}

       
//	std::clog << "Stream pos after encoding = "
//		  << xdr_getpos (&xdr) << std::endl;
	size_t datasize = buf.pos;
	ByteBuffer answer (buf.data, datasize);
	// 'answer' will delete the data buffer later when done with it

	return answer;
	
    }



    void decode (ByteBuffer bytes) {

	//               pos   cap           data
	quick_buf buf = { 0, bytes.len(), bytes.data() };
	
	xdrrec_create (&xdr,
		       1, BUFSIZE, // don't need a write buffer
		       (char*)&buf, read_xdrrec, NULL);
	xdr.x_op = XDR_DECODE;
	
	memset (&x, 0, sizeof(x));
	
	// memory may be allocated for our struct
	should_free_struct = true;

	// FIXME: don't really know how xdrrec_skiprecord works here,
	// but it appears (emprically) to be needed
	if ( xdrrec_skiprecord(&xdr) == 0 ) {
	    throw xdr_exception ("Decoding: skiprecord", errno);
	}
	
	if ( ! Filter (&xdr, &x) ) {
	    throw xdr_exception (std::string("Decoding ") + typeid(T).name(),
				 errno);
	}
    }
#endif // _XDR_USE_XDRREC_STREAM


    ByteBuffer encode () const throw (xdr_exception) {

	const size_t MAXSIZE = 128 * (1<<10); // 128K
	size_t size = BUFSIZE;

	bool done = false;

	ByteBuffer answer;

	errno = 0;		// for some predictability
	
	while (!done) {
	    answer = ByteBuffer (new byte[size], size);
	
	    xdrmem_create (&xdr,
			   answer.cdata(), answer.len(),
			   XDR_ENCODE);

	    if ( filter_encode( &xdr, &x) == 0 ) {
		if (size < MAXSIZE) {
		    size *= 2;	// increase size
		}
		else {		// give up
		    throw xdr_exception
			(std::string("Encoding ") + typeid(T).name(), errno);
		}
	    }
	    else {
		done = true;	// success!!
	    }
	}

//	std::clog << "Stream pos after encoding = "
//		  << xdr_getpos (&xdr) << std::endl;
	answer.len() = xdr_getpos (&xdr);

	return answer;
    }



    void decode (ByteBuffer buf) {
	memset (&x, 0, sizeof(x));
	
	xdrmem_create (&xdr,
		       buf.cdata(), buf.len(),
		       XDR_DECODE);

	// memory may be allocated for our struct
	should_free_struct = true;

	if ( ! Filter (&xdr, &x) ) {
	    throw xdr_exception (std::string("Decoding ") + typeid(T).name(),
				 errno);
	}
    }


    ~XDRStruct () {
	if (should_free_struct) {
	    xdr.x_op = XDR_FREE;
	    Filter (&xdr, &x);
	}
	xdr_destroy (&xdr);
    }


private:

    // const correcting wrapper for encoding (which presumably doesn't touch the
    // value t
    static bool_t filter_encode (XDR* xdr, const T* t) {
	return Filter (xdr, const_cast<T*> (t));
    }
    

    mutable XDR xdr;
    bool should_free_struct;

};



//
// a wrapper function to do encoding
//

template <class T, bool_t (*Filter) (XDR*, T*)>
ByteBuffer encode_xdr (const T& x) {
    return XDRStruct<T,Filter> (x).encode();
}

#define ENCODE_XDR(T) encode_xdr<T, xdr_ ## T>



//
// a class to help with C++ marshalling
//
// see bucket.{h,cc} for example usage

// T is the type of the derived class
// it's just used to call free_xdr. so T has to implement a static
// function free_xdr
template <typename T, typename XDRType, bool_t (*Filter) (XDR*, XDRType*)>
class Serializable {
    
public:
    
    // these two are defined here and the same across all derived classes
    void reconstruct (const ByteBuffer& serial) throw (xdr_exception) {
	XDRStruct<XDRType,Filter> xdr;
	xdr.decode (serial);
	from_xdr (xdr.x);
    }
    
    ByteBuffer serialize () const throw (xdr_exception) {
	XDRType x;
	to_xdr (x);
	ByteBuffer answer = encode_xdr<XDRType,Filter> (x);
	T::free_xdr (x);
	return answer;
    }

    
    // these have to be supplied by the derived classes, to:
    // construct the object from its XDR type,
    // create the XDR type
    virtual void from_xdr (const XDRType& x)    = 0;
    virtual void to_xdr   (XDRType & o_x) const = 0;

//    virtual void free_xdr (XDRType & x) const   = 0;

    virtual ~Serializable () {}
};


#define SERIALIZABLE(T,XDR_T) Serializable<T, XDR_T, xdr_ ## XDR_T>


#endif // _XDRSTRUCT_H
