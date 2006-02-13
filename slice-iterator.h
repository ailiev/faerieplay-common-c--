// -*- c++ -*-

//#include <boost/range.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/type_traits/is_same.hpp>


OPEN_NS

template <class ContItr>	// the itr to the 1-D containers we're iterating over.
class slice_iterator
{

    // the 1-D container type
    typedef typename std::iterator_traits<ContItr>::value_type cont_t;
    
    // the nested types required for iterator_traits
#define GRAB_TYPEDEF(name) typedef typename std::iterator_traits<typename cont_t::iterator>::name name
    GRAB_TYPEDEF(iterator_category);
    GRAB_TYPEDEF(value_type);
    GRAB_TYPEDEF(difference_type);
    GRAB_TYPEDEF(pointer);
    GRAB_TYPEDEF(reference);
#undef GRAB_TYPEDEF

    
public:
    slice_iterator (const ContItr& conts,
		    unsigned slice)
	: _cont_itr (conts),
	  _slice (slice)
	{}

    reference operator* ()
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

    bool operator != (const slice_iterator& b)
	{
	    // 
	    assert (((void)"slice_iterator's in operator!= should have same slice number",
		     _slice == b._slice));

	    return _cont_itr != b._cont_itr;
	}


private:

    ContItr 	    _cont_itr;
    unsigned	    _slice;
};

    
CLOSE_NS
