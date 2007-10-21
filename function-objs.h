// -*- c++ -*-
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

#include <functional>
#include <utility>

#include <string>


#include <common/countarray.hpp>

#include <boost/array.hpp>

#ifndef _FUNCTION_OBJS_H
#define _FUNCTION_OBJS_H


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
    
private:
    result_type n;
};


// generator class to produce a newly allocated ByteBuffer of a given size
struct alloc_buffer : public generator<ByteBuffer>
{
    alloc_buffer (size_t size) : size(size) {}

    ByteBuffer operator () () { return ByteBuffer (size); }

    size_t size;
};



// template <class Pair>
// struct get_first : public std::unary_function<Pair, typename Pair::first_type>
// {
//     typename Pair::first_type & operator() (Pair & p) const
// 	{
// 	    return p.first;
// 	}
// };
template <class T1, class T2>
struct get_first : public std::unary_function<std::pair<T1,T2>, T1>
{
    const T1& operator() (const std::pair<T1,T2> & p) const
	{
	    return p.first;
	}

    T1& operator() (std::pair<T1,T2> & p) const
	{
	    return p.first;
	}
};
 
template <class T1, class T2>
struct get_second : public std::unary_function<std::pair<T1,T2>, T2>
{
    const T2& operator() (const std::pair<T1,T2> & p) const
	{
	    return p.second;
	}
    T2& operator() (std::pair<T1,T2> & p) const
	{
	    return p.second;
	}
};


template <class Cont>
struct deref : public std::unary_function<Cont, typename Cont::value_type>
{
    typedef typename Cont::value_type val_t;
    
    deref (unsigned idx)
	: idx(idx)
	{}
    
    const val_t& operator() (const Cont& c) const
	{
	    return c[idx];
	}

    val_t& operator() (Cont& c) const
	{
	    return c[idx];
	}

    unsigned idx;
};



// function object to convert a scalar to an array of N items with that scalar
// in each index.
template <class T, size_t N>
struct scalar2array : public std::unary_function<T, boost::array<T,N> >
{
    boost::array<T,N> operator () (const T& t) const
	{
	    boost::array<T,N> answer;
	    for (unsigned i=0; i < N; i++)
	    {
		answer[i] = t;
	    }
	    return answer;
	}
};


//
// function object to delete pointers
//

template <class T>
struct deleter : public std::unary_function<T*, void>
{

    void operator() (T * t) {
	delete t;
    }

private:
    T * _obj;
};




//
// object and helper functions to print an unsigned integer value in binary.
//

template <class I>
struct bin_printer {

    bin_printer (I x, size_t min_width)
	: x(x),
	  min_width (min_width)
	{}
    
    
    I x;
    size_t min_width;
};


template<class I>
bin_printer<I> bin_print (I x, size_t min_width=16)
{
    return bin_printer<I> (x, min_width);
}


template <class I>
std::ostream& operator<<
    (std::ostream& os, const bin_printer<I>& pr)
{
    std::string buf;
    unsigned w = 0;

    I x = pr.x;

    do
    {
	buf += '0' + (x & 1U);
	x >>= 1U;
	++w;
    }
    while (x > 0 || w < pr.min_width);

    reverse (buf.begin(), buf.end());

    os << buf;

    return os;
}




// template <class Pair>
// struct get_second : public std::unary_function<Pair, typename Pair::second_type>
// {
//     typename Pair::second_type & operator() (Pair & p) const
// 	{
// 	    return p.second;
// 	}
// };



// // adapt a generator into a unary function that ignores its param
// template <class AdaptableGenerator>
// const 


#endif /* _FUNCTION_OBJS_H */
