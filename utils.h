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



#include <iostream>
#include <string>
#include <list>
#include <iterator>
#include <vector>
#include <algorithm>
#include <functional>

#include <boost/optional/optional.hpp>

#include <stddef.h>		// for size_t
#include <stdio.h>
#include <string.h>		// for memcpy and strerror
#include <assert.h>

#include <sys/types.h>		// for mode_t


#include "utils-types.h"
#include "utils-macros.h"

#include "countarray.hpp"
#include "exceptions.h"
#include "comm_types.h"


#ifndef _COMMON_UTILS_H
#define _COMMON_UTILS_H


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



// function to copy bytes between ByteBuffer's
inline void bbcopy (ByteBuffer & dest, const ByteBuffer& src) {
    assert (dest.len() >= src.len());
    (void) memcpy (dest.data(), src.data(), src.len());
}



// for printing the current errno to an ostream, eg:
// cerr << "Bad error: " << errmsg << endl;
std::ostream & errmsg (std::ostream & os);


// a file descriptor
typedef int fd_t;


// round up to boundary
unsigned round_up (unsigned x, unsigned boundary);


// unsigned ilog2 (unsigned x);


std::string itoa (int  n);



// InputIterator must have element type B
template <class A, class B, class InputIterator>
A foldl (std::binary_function<A, B, A> f, const A& f0,
	 InputIterator start, InputIterator end)
{
    B intermed = f0;
    for (InputIterator i = start; i != end; i++) {
	intermed = f (intermed, *i);
    }

    return intermed;
}


// InputIterator must have element type convertible to T
template <class T, class InputIterator>
T sum (InputIterator start, InputIterator end)
{
    return foldl<T, T, InputIterator> (std::plus<T>(), static_cast<T> (0),
				       start, end);
}

// name from Haskell's maybe type
// note that this invokes a deep copy, so may not be appropriate for bit T's
template <class T>
boost::optional<T> Just (T const& t)
{
    return boost::optional<T> (t);
}

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



// is an element in a list?
template <class ElemT, class Cont>
bool elem (const ElemT & e, const Cont& l)
{
    return std::find (l.begin(), l.end(), e) != l.end();
}

// also with iterators
// template <class ElemT, class Cont>
// bool elem (const ElemT & e,
// 	   const Cont)
// {
//     return std::find (start, l.end(), e) != l.end();
// }



/// class to indicate that a derived class should be used by only one
/// entity at a time: copies are allowed, but the original becomes
/// useless after a copy.
//
/// TODO: there is no way to enforce these semantics, perhaps can add a runtime
/// loud warning in here? can't do that either!
class linear {
};


/// print the current epoch time in seconds to an ostream.
std::ostream & epoch_time (std::ostream & os);


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



//
// given two unary functions, give a function which applies f1 to the first of a
// pair, f2 to the second, and returns the resulting pair
//
template <class UnaryFunction1, class UnaryFunction2>
class applyer_to_pair :
    public std::unary_function<std::pair<typename UnaryFunction1::argument_type,
					 typename UnaryFunction2::argument_type>,
			       std::pair<typename UnaryFunction1::result_type,
					 typename UnaryFunction2::result_type> >
{
     typedef applyer_to_pair<UnaryFunction1,UnaryFunction2> this_t;
     
 public:
     
     applyer_to_pair (UnaryFunction1 & f1, UnaryFunction1 & f2)
	 : f1(f1),
	   f2(f2)
	 {}
     
     typename this_t::result_type
     operator() (const typename this_t::argument_type& p)
	 {
	     return make_pair (f1(p.first), f2(p.second));
	 }
     
     UnaryFunction1 f1;
     UnaryFunction2 f2;
 };

template <class UF1, class UF2>
inline applyer_to_pair<UF1,UF2>
apply_to_pair (UF1 & f1, UF2 & f2)
{
    return applyer_to_pair<UF1,UF2> (f1, f2);
}


// function to return the address of its paramater
template <class T>
struct addressof : public std::unary_function<T, T*>
{
    T*
    operator() (T & x) const
	{ return &x; }

    const T*
    operator() (const T& x) const
	{ return &x; }
};


// identity is copied from struct _Identity in stl_functional.h in the GNU C++
// library
template <class _Tp>
struct identity : public std::unary_function<_Tp,_Tp>
{
    _Tp&
    operator()(_Tp& __x) const
	{ return __x; }
    
    const _Tp&
      operator()(const _Tp& __x) const
	{ return __x; }
};

template <class Arg1, class Arg2> // Arg2 is also the result type
struct project2nd : public std::binary_function<Arg1,Arg2,Arg2>
{
    Arg2&
    operator()(Arg1& x, Arg2& y) const
	{ return y; }
    
    const Arg2&
      operator()(const Arg1& x, const Arg2& y) const
	{ return y; }
};


template <class T>
struct scalar2pair : public std::unary_function<T, std::pair<T,T> >
{
    std::pair<T,T> operator() (const T& t) const
	{
	    return std::make_pair(t, t);
	}
};


// an analogue to unary_function etc.
template <class Result>
struct generator {
    typedef Result result_type;
};


// a generator of const values
template <class T>
struct fconst : public generator<T>
{
    fconst (const T& t)
	: t(t)
	{}

    const T& operator() () const
	{
	    return t;
	}

    T t;
};

template <class T>
fconst<T>
make_fconst (const T& t)
{
    return fconst<T> (t);
}



// a generator class to generate a stream of consecutive integers
struct counter : public generator<index_t>
{
    counter(result_type start) : n(start) {}
    result_type operator() () { return n++; }
    
    result_type n;
};


// generator class to produce a newly allocated ByteBuffer of a given size
struct alloc_buffer : public generator<ByteBuffer>
{
    alloc_buffer (size_t size) : size(size) {}

    ByteBuffer operator () () { return ByteBuffer (size); }

    size_t size;
};



// // adapt a generator into a unary function that ignores its param
// template <class AdaptableGenerator>
// const 

#endif // _COMMON_UTILS_H
