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


#include "utils.h"

#include <assert.h>

#include <iostream>

using namespace std;



int main (int argc, char* argv[])
{

    {
	// test lgN
	assert (argc > 1);

	int N = atoi(argv[1]);

	cout << "lgN_ceil " << N << " = " << lgN_ceil(N) << endl;
	cout << "lgN_floor " << N << " = " << lgN_floor(N) << endl;
    }

    {
	// test print_seq
	vector<int> is;
	for (int i=0; i < 14; ++i) {
	    is.push_back(i);
	}
	cout << print_seq(is.begin(), is.end()) << endl;
	
// 	cout << "Printing list of integers:" << endl
// //	     << print_seq(is) << endl
// 	     << "And via the iterators:" << endl
// 	     << print_seq(is.begin(), is.end()) << endl;
	
    }

    {
	// test split (and print_seq :))
	cout << "Testing split()" << endl;
	string ins[] = { "a:bbc:dse:frtg:df",
			 "def:erf3:",
			 "def:erf3::",
			 ":abc",
			 "abc:",
			 ":",
			 ""
	};
	for (int i=0; i<ARRLEN(ins); i++) {
	    cout << '"' << ins[i] << '"' << "->";
	    cout << print_seq (split (":", ins[i]));
	    cout << endl;
	}
    }

    return 0;
}
