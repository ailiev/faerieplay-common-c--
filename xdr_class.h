// -*- c++ -*-
// xdr_class.h
// alex iliev, nov 2002
// class to wrap an XDR-generated struct


#ifndef _XDRSTRUCT_H
#define _XDRSTRUCT_H


#include <iostream>

#include <rpc/xdr.h>

#include <common/utils.h>
#include <common/consts.h>



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
    

    
    // FIXME: for now we hardwire the max size a struct may encode to for
    // encoding, better to not do this, and use the xdrrec_create stream type
   ByteBuffer encode () const throw () {
	ByteBuffer answer (new byte[BUFSIZE], BUFSIZE);

	xdrmem_create (&xdr,
		       reinterpret_cast<char*> (answer.data()), answer.len(),
		       XDR_ENCODE);

	if ( ! filter_encode (&xdr, &x) ) {
	    // TODO ...
	    perror ("XDR encoding structure");
	}

//	std::clog << "Stream pos after encoding = "
//		  << xdr_getpos (&xdr) << std::endl;
	answer.len() = xdr_getpos (&xdr);

	return answer;
    }



    void decode (ByteBuffer buf) {
	memset (&x, 0, sizeof(x));
	
	xdrmem_create (&xdr,
		       reinterpret_cast<char*> (buf.data()), buf.len(),
		       XDR_DECODE);

	// memory may be allocated for our struct
	should_free_struct = true;

	if ( ! Filter (&xdr, &x) ) {
	    // TODO ...
	    perror ("XDR decoding structure");
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


#endif // _XDRSTRUCT_H
