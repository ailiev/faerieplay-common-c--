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
#include <vector>
#include <algorithm>


#include <stddef.h>		// for size_t
#include <stdio.h>
#include <string.h>		// for memcpy and strerror
#include <assert.h>

#include <sys/types.h>		// for mode_t



// HACK: this makes the byte type available to countarray.hpp
typedef unsigned char byte;


#include "countarray.hpp"
#include "exceptions.h"
#include "comm_types.h"



typedef CountedByteArray ByteBuffer;


// build the dir structure which contains 'name' - not the actual top object
// though. give all new directories access mode 'mode' as in chmod(2)
void builddirs (const std::string& name, mode_t mode)
    throw (io_exception);



void readfile (FILE * fh, std::string& into) throw (io_exception);

// disabled for now, see comment in utils.cc
//void readfile (std::istream& is, std::string& into) throw (io_exception);


// and a bit higher level:
ByteBuffer readfile (const std::string& name) throw (io_exception);

void writefile (const std::string& name,
		const ByteBuffer& data)
    throw (io_exception);


ByteBuffer realloc_buf (const ByteBuffer&, size_t new_size);


// work out a floor/ceil of lg(N)
unsigned lgN_floor (int N);
unsigned lgN_ceil (int N);

// unsigned integer division while rounding up
inline unsigned divup (unsigned a, unsigned b) {
    return a % b == 0 ? a/b : a/b + 1;
}



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



// a generator class to generate a stream of consecutive integers
struct counter
{
  typedef index_t result_type;

  counter(result_type start) : n(start) {}
  result_type operator() () { return n++; }

  result_type n;
};


// generator class to produce a newly allocated ByteBuffer of a given size
struct alloc_buffer {
    typedef ByteBuffer result_type;
    
    alloc_buffer (size_t size) : size(size) {}

    ByteBuffer operator () () { return ByteBuffer (size); }

    size_t size;
};


// function to copy bytes between ByteBuffer's
inline void bbcopy (ByteBuffer & dest, const ByteBuffer& src) {
    assert (dest.len() >= src.len());
    (void) memcpy (dest.data(), src.data(), src.len());
}


#define GETBIT(a,i)   ( (a) & (1U << (i)) ) >> (i)
// assumes without check that b is 0 or 1
#define SETBIT(a,i,b) (a) |= (b) << (i)



#define FOREACH(i,v) for (typeof((v).begin()) i = (v).begin(); \
                          i != (v).end(); \
                          i++)

// for printing the current errno to an ostream, eg:
// cerr << "Bad error: " << errmsg << endl;
std::ostream & errmsg (std::ostream & os);


// a file descriptor
typedef int fd_t;


// round up to boundary
unsigned round_up (unsigned x, unsigned boundary);


// unsigned ilog2 (unsigned x);


// some logging stuff

#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

std::string itoa (int  n);


// It has to be an iterator of (ByteBuffer)
template <class It>
ByteBuffer concat_bufs (It start, It end)
{
    ByteBuffer answer;

    int sum = 0;
    for (It it = start; it != end; it++) {
	sum += it->len();
    }

    // allocate the new one
    answer = ByteBuffer (sum);

    int off = 0;
    for (It it = start; it != end; it++) {
	memcpy (answer.data() + off, it->data(), it->len());
	off += it->len();
    }
    
    return answer;
}


#define ARRLEN(a) ( sizeof(a) / sizeof((a)[0]) )


// is an element in a list?
template <class ElemT>
bool elem (const ElemT & e, const std::list<ElemT>& l) {
    return std::find (l.begin(), l.end(), e) != l.end();
}


// class to indicate that a derived class should be used by only one entity at a
// time: copies are allowed, but the original becomes useless after a copy.
//
// TODO: there is no way to enforce these semantics, perhaps can add a runtime
// loud warning in here
class linear {
};


// return: tuple of iterators
// param: iterator over tuples
// template <class It1, class It2>
// boost::tuple<It1, It2> unzip ()




//  Example 4(b): Improved solution for auto_ptr exception safety
// from http://www.gotw.ca/gotw/056.htm
//
template<class T>
std::auto_ptr<T> auto_ptr_new()
{
    return std::auto_ptr<T>( new T );
}

template<class T, class Arg1>
std::auto_ptr<T> auto_ptr_new( const Arg1& arg1 )
{
    return std::auto_ptr<T>( new T( arg1 ) );
}

template<class T, class Arg1, class Arg2>
std::auto_ptr<T> auto_ptr_new( const Arg1& arg1,
			       const Arg2& arg2 )
{
    return std::auto_ptr<T>( new T( arg1, arg2 ) );
}


// etc.

#endif // _UTILS_H
