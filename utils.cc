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
#include <string>

#include <stdio.h>
#include <math.h>

#include <sys/stat.h>
#include <sys/types.h>


#include "consts.h"
#include "utils.h"


using namespace std;



// build the dir structure which contains 'name' - not the actual top object
// though
int builddirs (const string& name, mode_t mode) {

    string dirname = name.substr (0, name.rfind (DIRSEP));

    clog << "Making dir " << dirname << endl;
    
    int status = mkdir (dirname.c_str(), mode);
    if (status == 0 || errno != ENOENT) return status; // success or
						       // unrecoverable failure

    // didnt find the parent dir - recurse
    status = builddirs (dirname, mode);
    if (status != 0) return status;

    // and redo the mkdir here
    status = mkdir (dirname.c_str(), mode);
    return status;
}




void readfile (FILE * fh, string& into) throw (ios::failure) {

    char buf[512];
    int read;

    into.clear();
    while ( (read = fread (buf, 1, sizeof(buf), fh)) > 0 ) {
	into.append (buf, read);
    }

    if (ferror(fh)) {
	throw (ios::failure (string ("Reading file: ") + strerror(errno)));
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

