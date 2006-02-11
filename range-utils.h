// -*- c++ -*-

#include <ostream>
#include <iterator>		// for ostream_iterator

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/iterator.hpp>
#include <boost/range/iterator_range.hpp>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/counting_iterator.hpp>
//#include <boost/iterator/zip_iterator.hpp>

#include <boost/none.hpp>


#include <boost/optional.hpp>

#include "utils-macros.h"


#ifndef _RANGE_UTILS_H
#define _RANGE_UTILS_H


OPEN_NS




template <class Range, class UnaryFunction>
#define PARENTTYPE \
boost::iterator_range<boost::transform_iterator<UnaryFunction, \
                                                typename boost::range_const_iterator<Range>::type> >
struct transform_range : public PARENTTYPE
{
    typedef PARENTTYPE parent_t;

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


template <class Range>
void print_range (std::ostream& os, const Range& r)
{
    typedef typename boost::range_value<Range>::type val_t;
    
    std::copy (boost::const_begin (r), boost::const_end (r),
	       std::ostream_iterator<val_t>(os, "\n"));
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
