/*
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
 * utils.cc
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



/* work out a floor of lg(N), by repeated right shifting by 1 */
int lgN_floor (int N) {

    int i;
    for (i = 0; N > 1; N >>= 1, i++)
	;

    return i;
}

