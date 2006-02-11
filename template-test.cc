#include <vector>
#include <functional>

#include <iostream>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/range.hpp>
#include <boost/bind.hpp>

#include "range-utils.h"

#include "slice-iterator.h"

using namespace std;
using namespace pir;


void test_slice_iterator ();


int main ()
{
    test_slice_iterator ();
    
    return 0;
}


void test_slice_iterator ()
{
    vector<vector<int> > v (10);

    for (unsigned i=0; i < v.size(); i++) {
	v[i].resize (10);
	for (unsigned j = 0; j < v[i].size(); j++) {
	    v[i][j] = i * j;
	}
    }

    typedef slice_iterator<vector<vector<int> >::iterator > slice_itr_t;
    
    slice_itr_t s_begin (v.begin(), 9);
    slice_itr_t s_end (v.end(), 9);

    for (slice_itr_t si = s_begin; si != s_end; si++)
    {
	cout << "number " << *si << endl;
    }
}

#if 0
void test_concat_iterator ()
{
    vector<vector<int> > v (10);

    for (unsigned i=0; i < v.size(); i++) {
	v[i].resize (10);
	for (unsigned j = 0; j < v[i].size(); j++) {
	    v[i][j] = i * j;
	}
    }

    typedef concat_iterator<vector<vector<int> >::iterator > concat_itr_t;

    concat_itr_t c_begin (v.begin());
    concat_itr_t kuku;
}
#endif


int add (const int& x, const int& y)
{
    return x+y;
}

void test_range_utils ()
{
    vector<int> v (10, 13);

    counting_range<int> cr (0,10);

    print_range (cout, make_transform_range (v, bind1st (plus<int>(), 12)));
    print_range (cout, make_transform_range (v, boost::bind (add, 12, _1)));
    print_range (cout, make_transform_range (cr, boost::bind (add, 12, _1)));
}

