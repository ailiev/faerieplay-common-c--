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

/* -*- c++ -*-
 * utils.h
 * alex iliev, nov 2002
 * various useful declarations
 */


#ifndef _UTILS_H
#define _UTILS_H

#include <iostream>

#include <stddef.h>		// for size_t
#include <stdio.h>

#include <string>

#include "countarray.hpp"
// #include "countedptr.h"

#include "comm_types.h"


typedef unsigned char byte;


typedef CountedArray<byte> ByteBuffer;


void readfile (FILE * fh, std::string& into) throw (std::ios::failure);


ByteBuffer realloc_buf (const ByteBuffer&, size_t new_size);


#endif // _UTILS_H
