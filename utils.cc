/*
 * utils.c
 * alex iliev, nov 2002
 */

#include <iostream>

#include <stdio.h>
#include <math.h>

#include <string>

#include "utils.h"


using namespace std;



void readfile (FILE * fh, string& into) throw (ios::failure) {

    char buf[512];
    int read;

    into.clear();
    while ( (read = fread (buf, 1, sizeof(buf), fh)) > 0 ) {
	into += string (buf, read);
    }

    if (ferror(fh)) {
	throw (ios::failure ("Reading file: " + string (strerror(errno))));
    }
}


ByteBuffer
realloc_buf (const ByteBuffer& old, size_t new_size) {
    return ByteBuffer
	( static_cast<byte*>
	  (memcpy (new byte[new_size], old.data(), min (old.len(),new_size))),
	  new_size );
}

