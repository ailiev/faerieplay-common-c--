#define ARRLEN(a) ( sizeof(a) / sizeof((a)[0]) )


#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

/// produce an integer with only the i-th bit set.
#define BITAT(i)	(1U << (i))
/// a bit mask from #i to #j inclusive.
#define BITMASK(i,j)	( ((1U << (j-i+2)) - 1) << i )
/// get the i-th bit of a.
#define GETBIT(a,i)	( (a) & BITAT(i) ) >> (i)
/// extract masked bits
#define GETBITS(x,i,j) ( ((x) & BITMASK(i,j)) >> (i) )
/// set the #i-th bit of #a to #b
// (b&1) to make sure we get either a 0 or one.
#define SETBIT(a,i,b)	(a) |= ((b) & 1) << (i)




#define FOREACH(i,v) for (typeof((v).begin()) i = (v).begin(); \
                          i != (v).end(); \
                          i++)
