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


#include "utils-macros.h"

#include <iostream>

using namespace std;

int main (int argc, char* argv[])
{
    unsigned x = atoi (argv[1]),
	i = atoi (argv[2]),
	j = atoi (argv[3]);

    cout << uppercase;
    
    cout << "BITMASK (" << i << ":" << j
	 << ") = 0x" << hex << BITMASK(i,j) << endl;
    
    cout << "GETBITS " << dec << i << " to " << j
	 << " of 0x" << hex << x
	 << " = 0x" << GETBITS (x,i,j) << endl;

    cout << "GENBITMASK with b=0 is 0x" << hex << GENBITMASK(i,j,0)
	 << " and with b=1 is 0x" << GENBITMASK(i,j,1) << endl;

    cout << "SETBITS to 0's is 0x" << hex << SETBITS(x,i,j,0)
	 << " and to 1's is 0x" << SETBITS(x,i,j,1) << endl;

    cout << "sign extend 0x" << x
	 << " from " << dec << i << " to " << j << " bits = 0x"
	 << hex << SIGNEXTEND(x,i,j) << endl;
}
