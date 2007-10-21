/*
 * Library routines for the Dartmouth CS Private Information Retrieval and
 * Writing (PIR/W) library.
 *
 * Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu>
 *
 * All rights reserved.
 *
 * This code is released under a BSD license.
 * Please see LICENSE.txt for the full license and disclaimers.
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com)
*/


#include <vector>
#include <functional>

#include <iostream>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/range.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>

#include "range-utils.h"

#include "slice-iterator.h"

using namespace std;
using namespace pir;
using namespace boost;


void test_slice_iterator ();

void test_slice_range ();



int main ()
{
    test_slice_range();

    return 0;
}





void test_slice_range ()
{
    vector<vector<int> > v (10);

    for (unsigned i=0; i < v.size(); i++) {
	v[i].resize (10);
	for (unsigned j = 0; j < v[i].size(); j++) {
	    v[i][j] = i * j;
	}
    }

    
    slice_range<vector<vector<int> >::iterator> slice (v, 5);
//    typedef boost::range_iterator< slice_range<vector<vector<int> >::iterator>
//    >::type slice_itr_t;
    typedef slice_iterator < vector<vector<int> >::iterator > slice_itr_t;
    
    for (slice_itr_t si = boost::begin (slice);
	 si != boost::end (slice);
	 si++)
    {
	// do a modification through the slice iterator
	*si += 100;
    }

    for (slice_itr_t si = boost::begin (slice);
	 si != boost::end (slice);
	 si++)
    {
 	cout << "number " << (*si) << endl;   
    }

    vector<int> v2 (10);
    generate (v2.begin(), v2.end(), counter (12345678));
//     slice_range<vector<array<int,3> >::iterator> r (make_transform_range (v2, scalar2array<int,3>()),
// 						    2);
    copy_range (make_slice_range (make_transform_range (v2, scalar2array<int,3>()),
				  2),
		ostream_iterator<int> (cout, "\n"));
}


#if 0
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
#endif

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

