#include <vector>
#include <functional>

#include "range-utils.h"


int main ()
{
// 	    boost::iterator_range<boost::counting_iterator<size_t> > r =
// 		make_counting_range (0U, num_objs);

    typedef std::vector<size_t> vec_t;
    vec_t v;
	    
//	    typedef boost::counting_iterator<size_t> iter_t;
	    
    pir::iterator_range<vec_t::iterator > r1 =
	pir::make_iterator_range (v.begin(), v.end());
	    
    typedef vec_t::iterator iter_t;

    pir::iterator_range<boost::transform_iterator<std::binder1st<std::plus<size_t> >,
	iter_t> >
	r2 =
	pir::make_transform_range (r1, std::bind1st (std::plus<size_t>(), 10U));
	    
}
