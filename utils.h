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

/* -*- c++ -*-
 * utils.h
 * alex iliev, nov 2002
 * various useful declarations
 */


#ifndef _UTILS_H
#define _UTILS_H

#include <iostream>
#include <string>
#include <list>

#include <stddef.h>		// for size_t
#include <stdio.h>

#include <sys/types.h>		// for mode_t



// HACK: this makes the byte type available to countarray.hpp
typedef unsigned char byte;


#include "countarray.hpp"

#include "comm_types.h"



typedef CountedByteArray ByteBuffer;


// build the dir structure which contains 'name' - not the actual top object
// though. give all new directories access mode 'mode' as in chmod(2)
void builddirs (const std::string& name, mode_t mode)
    throw (std::ios::failure);



void readfile (FILE * fh, std::string& into) throw (std::ios::failure);
void readfile (std::istream& is, std::string& into) throw (std::ios::failure);


ByteBuffer realloc_buf (const ByteBuffer&, size_t new_size);


/* work out a floor of lg(N), by repeated right shifting by 1 */
int lgN_floor (int N);



// quick function to return  a list iterator n steps further than the
// given one. this function clearly has complexity O(n)
template<typename ListIt>
ListIt
iterator_add (const ListIt& i, size_t n) {
    ListIt answer = i;
    for (size_t j=0; j < n; j++) {
	++answer;
    }
    return answer;
}

#endif // _UTILS_H
