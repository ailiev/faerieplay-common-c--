// -*- c++ -*-


#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/iterator.hpp>
#include <boost/range/iterator_range.hpp>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/none.hpp>


#include <boost/optional.hpp>

#include "utils-macros.h"


#ifndef _RANGE_UTILS_H
#define _RANGE_UTILS_H


OPEN_NS




template <class Range, class UnaryFunction>
boost::iterator_range <
    boost::transform_iterator <UnaryFunction,
			       typename boost::range_iterator<Range>::type>
>
make_transform_range (const Range & r1,
		      const UnaryFunction & f)
{
    return boost::make_iterator_range (
	boost::make_transform_iterator (boost::const_begin (r1), f),
	boost::make_transform_iterator (boost::const_end (r1), f)
	);
}


/// make an Iterator range which counts over consecutive integers
/// @param start the first number in the count.
/// @param end one past the last number in the count.
/// TODO: make sure that size() can be gotten in constant time with this
/// iterator. use gdb probably.
template <class Num>
boost::iterator_range<boost::counting_iterator<Num> >
make_counting_range (Num start, Num end)
{
    return boost::make_iterator_range (boost::make_counting_iterator (start),
				       boost::make_counting_iterator (end));
}


CLOSE_NS

#endif // _RANGE_UTILS_H
