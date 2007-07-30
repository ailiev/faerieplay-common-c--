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
