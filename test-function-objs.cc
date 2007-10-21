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


#include "function-objs.h"

#include <iostream>

#include <vector>
#include <algorithm>

using namespace std;


void test_bin_printer();
void test_deleter();


int main ()
{
    test_bin_printer();

    test_deleter();

    return 0;
}



void test_bin_printer()
{
    for (int i = 256; i < 512; i++)
    {
	cout << bin_print(i,9) << endl;
    }
}

void test_deleter ()
{
    vector<int*> v;

    for (int i=0; i < 20; i++) {
	v.push_back (new int(i*7));
    }

    for_each (v.begin(), v.end(), deleter<int>());
}
