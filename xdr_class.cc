/* xdr_class.cc
 * alex iliev, may 2003
 */

#include <stdlib.h>

#include "xdr_class.h"



#ifdef _TESTING_XDR_CLASS

#include "record_x.h"

int main () {
    ByteBuffer b;
    
    {
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
    }

    {
	XDR_STRUCT(Record_x) xdr2;
	
	xdr2.decode (b);
	
	std::cout << "result: " << xdr2.x.attributes->attr.name << "="
		  << xdr2.x.attributes->attr.value << std::endl;
    }
}
#endif // _TESTING_XDR_CLASS



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

    size_t toread = MIN (count, buf->capacity - buf->pos);

    memcpy (o_data, buf->data + buf->pos, toread);
    buf->pos += toread;

    return toread;
}

