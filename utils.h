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
