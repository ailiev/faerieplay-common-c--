#include "utils-macros.h"

#include <iostream>

using namespace std;

int main (int argc, char* argv[])
{
    unsigned x = atoi (argv[1]),
	i = atoi (argv[2]),
	j = atoi (argv[3]);

    cout << "BITMASK (" << i << ":" << j << ") = 0x"
	 << hex << BITMASK(i,j) << dec << endl;
    
    cout << "GETBITS " << i << " to " << j << " of 0x" << hex << x << " = 0x"
	 << GETBITS (x,i,j) << dec << endl;
}
