// -*- c++ -*-

#define ARRLEN(a) ( sizeof(a) / sizeof((a)[0]) )


#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

/// produce an integer with only the i-th bit set.
#define BITAT(i)	(1U << (i))

/// a bit mask from #i to #j inclusive.
/// this one gives a word-sized bitmask
#define BITMASK(i,j)	( (~(~0 << (j-i+1))) << i )

/// a bit mask from #i to #j inclusive.
/// @param x just used to determine the type of the mask to produce (same as x)
#define BITMASK_WITHTYPE(x,i,j) ( (~(~((typeof(x))(0)) << (j-i+1))) << i )

/// get the i-th bit of a.
#define GETBIT(a,i)	( (a) & BITAT(i) ) >> (i)

/// extract masked bits
#define GETBITS(x,i,j) ( ((x) & BITMASK_WITHTYPE(x,i,j)) >> (i) )

/// set the #i-th bit of #a to #b
// (b&1) to make sure we get exactly a 0 or 1.
// first mask out the bit there now, then apply ours.
#define SETBIT(a,i,b)	(a) = ((a) & ~BITAT(i)) | (((b) & 1) << (i))

// produce a bitmask with 0s outside the (i:j) range, and b's at (i:j)
#define GENBITMASK(i,j,b)   ( b ? BITMASK(i,j) : 0U )

// return x with bits i to j (inclusive) set to b
#define SETBITS(x,i,j,b)    ( ((x) & ~BITMASK(i,j)) | GENBITMASK(i,j,b) )

// the value is x sign-extended from fromsize to tosize, eg 26 bits to 32 bits.
#define SIGNEXTEND(x,fromsize,tosize) \
    SETBITS(x,fromsize,tosize-1,GETBIT(x,fromsize-1))


// concatenate 2 16-bit values (high halfword first) to give a 32-bit value
#define CONCAT_16BIT(x,y)  ( (x) << 16 | (y) )



#define BZERO(x) memset (&(x), 0, sizeof(x))

#define FOREACH(i,v) for (typeof((v).begin()) i = (v).begin(); \
                          i != (v).end(); \
                          i++)

#define OPEN_NS namespace pir {
#define CLOSE_NS }


#define OPEN_ANON_NS namespace {


//
// static initiliozations helpers
//

#define DECL_STATIC_INIT_WITH_DESTR(initstms,destrstms)	\
    class StaticInit {					\
    public:						\
	StaticInit () {					\
	    if (num_insts++ == 0) {			\
                initstms;				\
	    }						\
	}						\
        ~StaticInit () {				\
	    if (--num_insts == 0) {			\
		destrstms;				\
	    }						\
	}						\
	static int num_insts;				\
    };

/// within the context in question (a class or namespace), declare a class to do
/// the required static initialization or cleanup, just once for that context.
#define DECL_STATIC_INIT(initstms)		\
    DECL_STATIC_INIT_WITH_DESTR(initstms, ; )


/// in a global scope, but within the header in question, declare an instance
/// of the static initializer. it will be created once for each inclusion of the
/// header (ie. once per object file), but its init statements will be executed
/// only the first time.
#define DECL_STATIC_INIT_INSTANCE(Ctx)			\
    namespace {						\
        Ctx::StaticInit __ ## Ctx ## _ ## static_init;	\
    }

/// instantiate the static variable which counts how many instances of the
/// StaticInit class in context Ctx have been created.
/// Goes in global scope of any source file, so it appears just once in the
/// whole program.
#define INSTANTIATE_STATIC_INIT(Ctx)		\
        int Ctx::StaticInit::num_insts = 0;
