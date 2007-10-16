// -*- c++ -*-

/*
Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu>

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of the author nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

This product includes cryptographic software written by Eric Young
(eay@cryptsoft.com)

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//#include <boost/range.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include "utils-macros.h"


#ifndef _SLICE_ITERATOR
#define _SLICE_ITERATOR

OPEN_NS

template <class ContItr>	// the itr to the 1-D containers we're iterating
				// over.
class slice_iterator
{

public:
    
    // the 1-D container type
    typedef typename std::iterator_traits<ContItr>::value_type cont_t;

    // the scalar type
    // FIXME: doesn't work for builting array 
    typedef typename cont_t::value_type scalar_t;
    
    // the nested types required for iterator_traits
    typedef scalar_t value_type;
    typedef scalar_t* pointer;	// array<T,N> does not have the pointer typedef
				// for some reason.
    
    typedef scalar_t& reference; // FIXME: cont_t::reference does not work:
				 // operator* does not return an lvalue in that
				 // case.

    typedef typename std::iterator_traits<ContItr>::iterator_category iterator_category;
    typedef typename std::iterator_traits<ContItr>::difference_type   difference_type;

    
public:
    slice_iterator (const ContItr& conts,
		    unsigned slice)
	: _cont_itr (conts),
	  _slice (slice)
	{}

// default copy semantics work here.
//     slice_iterator (const slice_iterator& b)
// 	: _cont_itr (b._cont_itr),
// 	  _slice    (b._slice)
// 	{}

    slice_iterator ()
	: _slice (0)
	{}

    reference operator* ()
	{
	    return (*_cont_itr)[_slice];
	}

    const scalar_t& operator* () const
	{
	    return (*_cont_itr)[_slice];
	}

    // preincrement
    slice_iterator& operator++ ()
	{
	    ++_cont_itr;
	    return *this;
	}

    // postincrement
    slice_iterator operator++ (int)
	{
	    slice_iterator snapshot = *this;
	    _cont_itr++;
	    return snapshot;
	}

    bool operator != (const slice_iterator& b) const
	{
	    // FIXME: may make sense to make _slice a template parameter, but
	    // we're already facing template explosion!
	    assert (((void)"slice_iterator's in operator!= "
		     "should have same slice number",
		     _slice == b._slice));

	    return _cont_itr != b._cont_itr;
	}

    bool operator== (const slice_iterator& b) const
	{
	    return ! operator!= (b);
	}
    
    // FIXME: this can only be pulled off if _cont_itr is random access
    difference_type operator- (const slice_iterator& b) const
	{
	    return _cont_itr - b._cont_itr;
	}
    

private:

    ContItr 	    _cont_itr;
    const unsigned  _slice;
};


template <class ContItr>	// the itr to the 1-D containers we're iterating
				// over.
slice_iterator<ContItr>
make_slice_iterator (const ContItr& conts,
		     unsigned slice)
{
//    BOOST_STATIC_ASSERT (
    return slice_iterator<ContItr> (conts, slice);
}

CLOSE_NS


#endif // _SLICE_ITERATOR
