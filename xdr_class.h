/* -*- c++ -*- */
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

#ifdef VALGRIND
#include <valgrind/memcheck.h>
#endif

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
		("Encoding failed to allocate memory for quick_buf");
	}
	
	xdrrec_create (&xdr,
		       BUFSIZE, 1, // don't need a read buffer, so make
		                   // it 1 byte
		       (char*) &buf,
		       NULL, write_xdrrec);
       
	xdr.x_op = XDR_ENCODE;
       
	if ( ! Filter (&xdr, const_cast<T*> (&x)) ) {
	    throw xdr_exception (std::string("Encoding ") + typeid(T).name());
	}

	// tell it we're done writing and it should flush its buffer
	// to ours
	if ( xdrrec_endofrecord (&xdr, 1) == 0 ) {
	    throw xdr_exception
		("Flushing XDR stream failed");
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
	    throw xdr_exception ("Decoding: skiprecord");
	}
	
	if ( ! Filter (&xdr, &x) ) {
	    throw xdr_exception (std::string("Decoding ") + typeid(T).name());
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
	    answer = ByteBuffer (size);
#ifdef VALGRIND
	    // to catch unitialized use errors.
	    VALGRIND_MAKE_WRITABLE (answer.data(), answer.len());
#endif
	
	    xdrmem_create (&xdr,
			   answer.cdata(), answer.len(),
			   XDR_ENCODE);

	    if ( filter_encode( &xdr, &x) == 0 ) {
		if (size < MAXSIZE) {
		    size *= 2;	// increase size
		}
		else {		// give up
		    throw xdr_exception
			(std::string("Encoding ") + typeid(T).name());
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



    void decode (const ByteBuffer& buf) {
	memset (&x, 0, sizeof(x));
	
	// note: const_cast here is fine, as the data buffer should be
	// just for reading
	xdrmem_create (&xdr,
		       const_cast<char*> (buf.cdata()), buf.len(),
		       XDR_DECODE);

	// memory may be allocated for our struct
	should_free_struct = true;

	if ( ! Filter (&xdr, &x) ) {
	    throw xdr_exception (std::string("Decoding ") + typeid(T).name());
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
// a class to help with C++ marshalling by putting in implementations for
// reconstruct and serialize, and leaving just the XDR-type-dependent operations
// (to_xdr, from_xdr) to concrete classes
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
