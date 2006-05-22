#include "function-objs.h"

#include <iostream>

using namespace std;

void test_bin_printer();


int main ()
{
    test_bin_printer();

    return 0;
}



void test_bin_printer()
{
    for (int i = 256; i < 512; i++)
    {
	cout << bin_print(i,9) << endl;
    }
}

