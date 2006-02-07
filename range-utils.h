// -*- c++ -*-


#include <iterator>		// for std::iterator_traits
//#include <boost/range.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/counting_iterator.hpp>

#include <boost/optional.hpp>

#include "utils-macros.h"


#ifndef _RANGE_UTILS_H
#define _RANGE_UTILS_H


OPEN_NS


template <class Itr>
class iterator_range
{
public:

    typedef Itr type;
    typedef std::iterator_traits<Itr> traits;

    iterator_range (const Itr& begin, const Itr& end,
		    boost::optional<typename traits::difference_type> size = boost::none)
	: range_begin(begin),
	  range_end (end),
	  _size (size)
	{}

    typename traits::difference_type size()
	{
	    // FIXME: great doubts that this will work for all iterators. may
	    // have to incremenet from begin to end and count, in the worst
	    // case.
	    if (_size) return *_size;
	    else return range_end - range_begin;
	}

public:
    Itr range_begin, range_end;
    boost::optional<typename traits::difference_type> _size;
    

#if 0


    iterator_range (Itr& begin, Itr& end)
	: _begin(begin),
	  _end (end),
	  _current(begin)
	{}

    Itr begin ()
	{
	    return _begin;
	}

    Itr end ()
	{
	    return _end;
	}

    
    iterator_range operator++ (int)
	{
	    iterator_range now = *this;
	    ++_current;
	    return now;
	}
    
    typename traits::pointer
    operator-> ()
	{
	    return _current.operator->();
	}
    
    typename traits::reference
    operator* ()
	{
	    return *_current;
	}

    bool operator== (const iterator_range<Itr>& b)
	{
	    return _current == b._current;
	}

    bool operator!= (const iterator_range<Itr>& b)
	{
	    return _current != b._current;
	}

    typename traits::difference_type size ()
	{
	    if (_size) return *_size;
	    else return _end - _current;
	}


    
private:
    Itr _begin, _end, _current;

    boost::optional<typename traits::difference_type> _size;

#endif // 0

};


template <class Itr>
Itr range_begin (const iterator_range<Itr>& r)
{
    return r.begin_range;
}

template <class Itr>
Itr range_end (const iterator_range<Itr>& r)
{
    return r.end_range;
}

// FIXME: these are for now identical to the non-const ones
template <class Itr>
Itr const_range_begin (const iterator_range<Itr>& r)
{
    return r.begin_range;
}

template <class Itr>
Itr const_range_end (const iterator_range<Itr>& r)
{
    return r.end_range;
}





template <class Itr>
pir::iterator_range<Itr>
make_iterator_range (const Itr & begin, const Itr & end)
{
    return pir::iterator_range<Itr> (begin, end);
}



template <class Range, class UnaryFunction>
pir::iterator_range <boost::transform_iterator <UnaryFunction,
						typename Range::type> >
make_transform_range (const Range & r1,
		      const UnaryFunction & f)
{
    return pir::make_iterator_range (
	boost::make_transform_iterator (r1.range_begin, f),
	boost::make_transform_iterator (r1.range_end, f)
	);
}


// get a const range for a container
template <class Cont>
pir::iterator_range <typename Cont::const_iterator>
make_container_range (const Cont& c)
{
    // hopefully real containers all have fast size() functions
    return pir::make_iterator_range (c.begin(), c.end(), c.size());
}

// and non-const
template <class Cont>
iterator_range <typename Cont::iterator>
make_container_range (Cont& c)
{
    return pir::make_iterator_range (c.begin(), c.end(), c.size());
}


/// make an Iterator range which counts over consecutive integers
/// @param end one past the last number in the count
template <class Num>
iterator_range<boost::counting_iterator<Num> >
make_counting_range (Num start, Num end)
{
    return pir::make_iterator_range (boost::make_counting_iterator (start),
				     boost::make_counting_iterator (end));
}


CLOSE_NS

#endif // _RANGE_UTILS_H
