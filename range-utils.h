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

#include <ostream>
#include <iterator>		// for ostream_iterator and iterator_traits

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/iterator.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/value_type.hpp> // range_value<>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/counting_iterator.hpp>

#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/static_assert.hpp>

#include <boost/none.hpp>


#include <boost/optional.hpp>

#include "slice-iterator.h"

#include "utils-macros.h"
#include "function-objs.h"


#ifndef _RANGE_UTILS_H
#define _RANGE_UTILS_H


OPEN_NS


#define ASSERT_RANGE_OF(Range,Elem)							\
    BOOST_STATIC_ASSERT ((boost::is_same<typename boost::range_value<Range>::type,	\
			                 Elem>::value))


template <class Range, class UnaryFunction>
#define PARENTTYPE \
boost::iterator_range<boost::transform_iterator<UnaryFunction, \
                                                typename boost::range_const_iterator<Range>::type> >
struct transform_range : public PARENTTYPE
{
    typedef PARENTTYPE parent_t;

    typedef typename boost::range_const_iterator<Range>::type itr_t;

    // these typedefs appear to be missing from the parent class somehow
//    typedef typename std::iterator_traits<itr_t>::iterator_category iterator_category;
//    typedef typename std::iterator_traits<itr_t>::pointer pointer;
//    typedef typename std::iterator_traits<itr_t>::reference reference;
    
    transform_range (const Range & r,
		     const UnaryFunction & f)
	: parent_t (boost::make_transform_iterator (boost::const_begin (r), f),
		    boost::make_transform_iterator (boost::const_end (r), f))
	{}
};
#undef PARENTTYPE


template <class Range, class UnaryFunction>
transform_range<Range, UnaryFunction>
make_transform_range (const Range & r,
		      const UnaryFunction & f)
{
    return transform_range<Range, UnaryFunction>(r, f);
}



template <class Range, class OutputIterator>
void copy_range (const Range& r, const OutputIterator& out)
{
    // apparenly there is no value_type for an OutputIterator
//    ASSERT_RANGE_OF (Range, std::iterator_traits<OutputIterator>::value_type);
    
    std::copy (boost::const_begin(r), boost::const_end(r), out);
}

    
template <class Range>
std::ostream& print_range (std::ostream& os, const Range& r)
{
    typedef typename boost::range_value<Range>::type val_t;
    
    std::copy (boost::const_begin (r), boost::const_end (r),
	       std::ostream_iterator<val_t>(os, "\n"));

    return os;
}



template <class Num>
#define PARENTTYPE boost::iterator_range<boost::counting_iterator<Num> >
struct counting_range : public PARENTTYPE
{
    counting_range (Num start, Num end)
	: PARENTTYPE (boost::make_iterator_range (
			  boost::make_counting_iterator (start),
			  boost::make_counting_iterator (end)))
	{}
};
#undef PARENTTYPE


/// make an Iterator range which counts over consecutive integers
/// @param start the first number in the count.
/// @param end one past the last number in the count.
/// TODO: make sure that size() can be gotten in constant time with this
/// iterator. use gdb probably.
template <class Num>
counting_range<Num>
make_counting_range (Num start, Num end)
{
    return counting_range<Num> (start, end);
}


//
// slice range
//
template <class Iterator>
#define PARENTTYPE boost::iterator_range<slice_iterator<Iterator> >
// template <class Range>
// #define PARENTTYPE boost::iterator_range<				
//                       slice_iterator<					
//                         typename boost::range_iterator<Range>::type > >
struct slice_range : public PARENTTYPE
{
    template <class Range>
    slice_range (const Range& r, unsigned slice)
	: PARENTTYPE (make_slice_iterator (boost::const_begin(r), slice),
		      make_slice_iterator (boost::const_end(r), slice))
	{
	    ASSERT_RANGE_OF(Range, typename std::iterator_traits<Iterator>::value_type);
	}

    template <class Range>
    slice_range (Range& r, unsigned slice)
	: PARENTTYPE (make_slice_iterator (boost::begin(r), slice),
		      make_slice_iterator (boost::end(r), slice))
	{
	    ASSERT_RANGE_OF(Range, typename std::iterator_traits<Iterator>::value_type);
	}
};
#undef PARENTTYPE


/// make a range which is a slice into a range over 1-D random-access containers
template <class Range>
slice_range<typename boost::range_const_iterator<Range>::type>
make_slice_range (const Range& r, unsigned slice)
{
    return slice_range<typename boost::range_const_iterator<Range>::type> (r, slice);
}

template <class Range>
slice_range<typename boost::range_iterator<Range>::type>
make_slice_range (Range& r, unsigned slice)
{
    return slice_range<typename boost::range_iterator<Range>::type> (r, slice);
}




#if 0
template <class Iterator>
#define PARENTTYPE boost::iterator_range<						\
                      boost::transform_iterator<					\
                        deref<typename std::iterator_traits<Iterator>::value_type>,	\
                        Iterator > >
struct slice_range : public PARENTTYPE
{
    typedef typename std::iterator_traits<Iterator>::value_type cont_t;
    
    template <class Range>
    slice_range (Range& r, unsigned slice)
	: PARENTTYPE (boost::make_iterator_range (
			  boost::make_transform_iterator (boost::begin(r),
							  deref<cont_t>(slice)),
			  boost::make_transform_iterator (boost::end(r),
							  deref<cont_t>(slice))))
	{}

    template <class Range>
    slice_range (const Range& r, unsigned slice)
	: PARENTTYPE (boost::make_iterator_range (
			  boost::make_transform_iterator (boost::const_begin(r),
							  deref<cont_t>(slice)),
			  boost::make_transform_iterator (boost::const_end(r),
							  deref<cont_t>(slice))))
	{}

};
#undef PARENTTYPE

/// make a range which is a slice into a range over 1-D random-access containers
template <class Range>
slice_range<typename boost::range_iterator<Range>::type>
make_slice_range (Range& r, unsigned slice)
{
    return slice_range<typename boost::range_iterator<Range>::type>
	(r, slice);
}
template <class Range>
slice_range<typename boost::range_const_iterator<Range>::type>
make_slice_range (const Range& r, unsigned slice)
{
    return slice_range<typename boost::range_const_iterator<Range>::type>
	(r, slice);
}
#endif // 0




// range trait check
// based on code in boost/range_iterator.hpp

    //////////////////////////////////////////////////////////////////////////
    // default, for containers with ::value_type
    //////////////////////////////////////////////////////////////////////////
    
template< typename Cont, class T >
struct is_range_of : public boost::is_same<typename Cont::value_type, T>
{};


template <class T>
struct is_range_of<std::vector<T>, T > : boost::true_type
{};


#if 0
//////////////////////////////////////////////////////////////////////////
    // pair
    //////////////////////////////////////////////////////////////////////////

    template< typename Iterator >
    struct is_range_of< std::pair<Iterator,Iterator> >
    {
        typedef Iterator type;
    };
    
    template< typename Iterator >
    struct is_range_of< const std::pair<Iterator,Iterator> >
    {
        typedef Iterator type;
    };

    //////////////////////////////////////////////////////////////////////////
    // array
    //////////////////////////////////////////////////////////////////////////

    template< typename T, std::size_t sz >
    struct is_range_of< T[sz] >
    {
        typedef T* type;
    };

    template< typename T, std::size_t sz >
    struct is_range_of< const T[sz] >
    {
        typedef const T* type;
    };

#endif // 0



/// if Range points to values of type T, ::type will be void. Otherwise ::type
/// will cause a type error.
template <class Range, class T>
struct ensure_range_of
{
    typedef
//    typename boost::enable_if<is_range_of<Range,T> >::type
    typename boost::enable_if<boost::is_same<
	typename boost::range_value<Range>::type,
	T> >::type
    type;
};



#if 0
template <class R1, class R2>
boost::iterator_range <
    boost::zip_iterator <
    boost::tuple<typename boost::range_iterator<R1>::type,
		 typename boost::range_iterator<R2>::type>
>
>
zip_ranges (const boost::tuple<R1, R2> & rs)
{
    return boost::make_iterator_range (boost::make_zip_iterator (rs));
}
#endif

/// wrapper class to present a scalar as a boost::range with one element
/// see http://www.boost.org/libs/range/doc/boost_range.html#minimal_interface
template <class T>
struct scalar_range {

public:
    
    scalar_range (const T& t)
	: x(t)
	{}

    // the types required by boost::range
//    typedef T value_type;
    typedef T* iterator;
    typedef const T* const_iterator;
//    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    
    // the functions required by boost::range
    iterator begin ()
	{
	    return &x;
	}

    iterator end ()
	{
	    return &x + 1;
	}

    const_iterator begin() const
	{
	    return &x;
	}

    const_iterator end () const
	{
	    return &x + 1;
	}

    size_type size ()
	{
	    return 1U;
	}
    
private:
    T x;
};


template <class T>
scalar_range<T> make_scalar_range (const T&t)
{
    return scalar_range<T> (t);
}


CLOSE_NS

#endif // _RANGE_UTILS_H
