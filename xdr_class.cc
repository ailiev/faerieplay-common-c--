/*
 * Library routines for the Dartmouth CS Private Information Retrieval and
 * Writing (PIR/W) library.
 *
 * Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu> and
 * Sean W. Smith <sws@cs.dartmouth.edu>
 *
 * All rights reserved.
 *
 * This code is released under a BSD license.
 * Please see LICENSE.txt for the full license and disclaimers.
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com)
*/

/* xdr_class.cc
 * alex iliev, may 2003
 */

#include <algorithm>

#include <stdlib.h>

#include "xdr_class.h"



#ifdef TESTING_XDR_CLASS

#include "record_x.h"

int main () {
    ByteBuffer b;
    
    {
	string_x str = "kuku this is a message";
	b = ENCODE_XDR(string_x) (str);
	
#if 0
	attr_x a;
	a.name = "My name is here";
	a.value = "And this is my immense value";

	attrlist_x la;
	la.attr = a;
	la.next = NULL;
	
	Record_x ra;
	ra.name = "This is my name";
	ra.attributes = &la;
	
	std::cout << "orig: " << ra.attributes->attr.name << "="
		  << ra.attributes->attr.value << std::endl;
	
	XDR_STRUCT(Record_x) xdr (ra);
	
	b = xdr.encode();
#endif
    }

    {
	XDR_STRUCT(string_x) xdr2;
	xdr2.decode (b);

	std::cout << "result: " << xdr2.x << std::endl;
#if 0
	XDR_STRUCT(Record_x) xdr2;
	
	xdr2.decode (b);
	
	std::cout << "result: " << xdr2.x.attributes->attr.name << "="
		  << xdr2.x.attributes->attr.value << std::endl;
#endif
    }
}
#endif // TESTING_XDR_CLASS


ssize_t write_xdrrec (char * handle, char *data, int count) {
    
    quick_buf * buf = (quick_buf*) handle;

    if (buf->capacity < buf->pos + count) {
	// figure out the new size
	size_t newcap = buf->capacity * 2;
	while (newcap < buf->pos + count) {
	    newcap *= 2;
	}

	// reallocate and copy
	byte * newbuf = new byte [newcap];
	memcpy (newbuf, buf->data, buf->pos);
	delete [] (buf->data);
	buf->data     = newbuf;
	buf->capacity = newcap;
    }
    
    // copy the stream data into our space
    memcpy (buf->data + buf->pos, data, count);
    buf->pos += count;

    return count;
}



ssize_t read_xdrrec (char * handle, char * o_data, int count) {

    quick_buf * buf = (quick_buf*) handle;

    size_t toread = std::min (unsigned(count), buf->capacity - buf->pos);

    memcpy (o_data, buf->data + buf->pos, toread);
    buf->pos += toread;

    return toread;
}




