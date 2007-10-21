/* -*- c++ -*- */
/*
 * Library routines for the Dartmouth CS Private Information Retrieval and
 * Writing (PIR/W) library.
 *
 * Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu> and
 * Sean W. Smith <sws@cs.dartmouth.edu>
 *
 * All rights reserved.
 *
 * This code is released under a BSD license.
 * Please see LICENSE.txt for the full license and disclaimers.
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com)
*/

/*
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
#include <boost/shared_ptr.hpp>

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


///
/// Function object to hold a sequence (expressed as a pair of iterators) for
/// nice printing.
/// Format: (x1, x2, ..., xn)
/// NOTE: cannot use the boost::range printing operators, as they do not output
/// separators, which seems pretty useless.
///
template <typename Itr>
struct seq_to_print
{
    seq_to_print (const Itr begin, const Itr end)
	: begin	(begin),
	  end	(end)
	{}

    const Itr begin, end;
};



///
/// Function to print a sequence (expressed as a pair of iterators) nicely.
/// Format: (x1, x2, ..., xn)
///
template <typename ConstItr>
seq_to_print<ConstItr> print_seq (const ConstItr begin,
				  const ConstItr end)
{
    return seq_to_print<ConstItr> (begin, end);
}



///
/// Function to print a sequence (with standard iterator methods) nicely.
///
template<typename Seq>
seq_to_print<typename Seq::const_iterator> print_seq (const Seq& seq)
{
    return seq_to_print<typename Seq::const_iterator> (seq.begin(), seq.end());
}

// seq_to_print just servers to hold the two iterators, and to idicate that this
// formatting function should be used.
template <typename Itr>
std::ostream& operator<< (std::ostream& os,
			  const seq_to_print<Itr>& seq)
{
    os << "(";
//    std::copy (seq.begin, seq.end, std::ostream_iterator<Elem> (os, ","));
    for (Itr i=seq.begin; i != seq.end; ++i) {
	os << *i;
	{
	    // print separator if this is not the last element.
	    Itr j = i;
	    if (++j != seq.end) {
		os << ",";
	    }
	}
    }
    os << ")";

    return os;

}



///
/// Function object to print an invocation of a function taking an ostream and
/// some other param.
///
/// TODO:


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


///
/// the object returned by 'memfun_adapt (member-function-address, object)'
/// has a single-const-parameter call operator which returns
/// object.memfun (arg)
/// eg:
///
/// class A { int f (int i); };
/// list<int> l;
/// ...
/// A a;
/// ...
/// transform (l.begin(), l.end(), l.begin(), memfun_adapt(A::f, a));
///
/// it's my best approximation in C++ for:
/// transform (l.begin(), l.end(), l.begin(), a->f)

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


///
/// Split a string at a given separator.
std::vector<std::string> split (const std::string& sep, const std::string& s);


// for printing the current errno to an ostream, eg:
// cerr << "Bad error: " << errmsg << endl;
std::ostream & errmsg (std::ostream & os);


// a file descriptor
typedef int fd_t;


///
/// round up to boundary
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
// note that this invokes a deep copy, so may not be appropriate for big T's
template <class T>
boost::optional<T> Just (T const& t)
{
    return boost::optional<T> (t);
}

// and a Nothing equivalent, but with a parameter to help with type inference
template <class T>
boost::optional<T> Nothing (T const&)
{
    return boost::optional<T> (boost::none);
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
/// loud warning in here? can't do that transparently either!
class linear {
};


/// print the current epoch time in seconds to an ostream.
std::ostream & epoch_time (std::ostream & os);

/// return the epoch time in seconds.
time_t epoch_secs ();


// print a list of ints
inline
std::ostream& operator<< (std::ostream & os, const std::list<int>& l)
{
    std::copy (l.begin(), l.end(),
	       std::ostream_iterator<int> (os, " "));

    return os;
}





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


template <class T>
boost::shared_ptr<T> shared_ptr_new ()
{
    return boost::shared_ptr<T> (new T);
}

template <class T, class A1>
boost::shared_ptr<T> shared_ptr_new (const A1& a1)
{
    return boost::shared_ptr<T> (new T (a1));
}

template <class T, class A1, class A2>
boost::shared_ptr<T> shared_ptr_new (const A1& a1, const A2& a2)
{
    return boost::shared_ptr<T> (new T (a1, a2));
}

template <class T, class A1, class A2, class A3>
boost::shared_ptr<T> shared_ptr_new (const A1& a1, const A2& a2, const A3& a3)
{
    return boost::shared_ptr<T> (new T (a1, a2, a3));
}

template <class T, class A1, class A2, class A3, class A4>
boost::shared_ptr<T> shared_ptr_new (const A1& a1, const A2& a2, const A3& a3,
				     const A4& a4)
{
    return boost::shared_ptr<T> (new T (a1, a2, a3, a4));
}



/// Erase elements matching pred from seq. Will reduce seq.size() if any
/// elements match.
template <class Seq, class Pred>
typename Seq::iterator
seq_erase_if (Seq & seq, Pred pred)
{
    return seq.erase (std::remove_if (seq.begin(), seq.end(), pred),
		      seq.end());
}


#endif // _COMMON_UTILS_H
