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
#include <iterator>


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



// quick function to return  an iterator n steps further than the
// given one. this function has complexity O(n) on list iterators, and constant
// on random-access iterators
template<typename Iterator>
Iterator
iterator_add (const Iterator& i, size_t n) {
    Iterator answer = i;
    std::advance (answer, n);
    return answer;
}


// the object returned by 'memfun_adapt (member-function-address, object)'
// has a single-const-parameter call operator which returns
// object.memfun (arg)
// eg:
//
// class A { int f (int i); };
// list<int> l;
// ...
// A a;
// ...
// transform (l.begin(), l.end(), l.begin(), memfun_adapt(A::f, a));
//
// it's my best approximation in C++ for:
// transform (l.begin(), l.end(), l.begin(), a->f)

template <class RetType, class ObjType, class ArgType>
struct memfun_adapter : public std::unary_function<ArgType, RetType> {
    explicit memfun_adapter ( RetType (ObjType::*func) (const ArgType&),
			      ObjType & obj )
	: _function (func),
	  _obj      (obj)
	{}

    RetType operator () (const ArgType& arg) {
	return (_obj.*_function) (arg);
    }
    
private:
    RetType (ObjType::*_function) (const ArgType&);
    ObjType & _obj;
};


template <class RetType, class ObjType, class ArgType>
inline memfun_adapter<RetType,ObjType,ArgType>
memfun_adapt (RetType (ObjType::*func) (const ArgType&),
	      ObjType & obj )
{
    return memfun_adapter<RetType,ObjType,ArgType> (func, obj);
}



#endif // _UTILS_H
