#include "utils.h"

#include <assert.h>

#include <iostream>

using namespace std;



int main (int argc, char* argv[])
{

    assert (argc > 1);

    int N = atoi(argv[1]);

    cout << "lgN_ceil " << N << " = " << lgN_ceil(N) << endl;
    cout << "lgN_floor " << N << " = " << lgN_floor(N) << endl;

    return 0;
}
