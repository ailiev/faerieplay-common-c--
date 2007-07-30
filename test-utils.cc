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
	cout << print_seq(is.begin(), is.end());
	
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
