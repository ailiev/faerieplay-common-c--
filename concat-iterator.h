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

#include <boost/range.hpp>
#include <iterator>


OPEN_NS

// this one iterates over a range of ranges, hopping from one to the other, like
// a concatenation.
template <class RangeItr> // the itr to the ranges we're concatenating
class concat_iterator
{
    // the type of the ranges we're concetenating
    typedef typename std::iterator_traits<RangeItr>::value_type range_t;
    // the type of the iterator we're "simulating"
    typedef typename boost::range_iterator<range_t>::type sub_itr_t;
    
    // the nested types required for iterator_traits
#define GRAB_TYPEDEF(name) \
    typedef std::iterator_traits<sub_itr_t>::name name

    GRAB_TYPEDEF(iterator_category);
    GRAB_TYPEDEF(value_type);
    GRAB_TYPEDEF(difference_type);
    GRAB_TYPEDEF(pointer);
    GRAB_TYPEDEF(reference);
#undef GRAB_TYPEDEF

    
public:
    concat_iterator (const RangeItr& ranges)
	: _range_itr (ranges),
	  _sub_itr (boost::begin (*_range_itr))
	{}

    const reference operator* () const
	{
	    return *_sub_itr;
	}

    reference operator* ()
	{
	    return *_sub_itr;
	}

    // preincrement
    concat_iterator& operator++ ()
	{
	    increment ();
	    return *this;
	}

    // postincrement
    concat_iterator operator++ (int)
	{
	    concat_iterator snapshot = *this;
	    increment ();
	    return snapshot;
	}

    bool operator == (const concat_iterator& b)
	{
	    return _range_itr == b._range_itr &&
		_sub_itr == b._sub_itr;
	}

    bool operator!= (const concat_iterator& b)
	{
	    return ! (operator== (b));
	}


private:
    void increment ()
	{
	    ++_sub_itr;
	    if (_sub_itr == boost::end (*_range_itr))
	    {
		_range_itr++;
		_sub_itr = boost::begin (*_range_itr);
	    }
	}
	    

    RangeItr 	    _range_itr;
    sub_itr_t	    _sub_itr;
};

    
CLOSE_NS
