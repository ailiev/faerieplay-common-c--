// -*- c++ -*-

#define ARRLEN(a) ( sizeof(a) / sizeof((a)[0]) )


#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

/// produce an integer with only the i-th bit set.
#define BITAT(i)	(1U << (i))
/// a bit mask from #i to #j inclusive.
//#define BITMASK(i,j)	( ((1U << (j-i+1)) - 1) << i )
#define BITMASK(i,j)	( (~(~0 << (j-i+1))) << i )
/// get the i-th bit of a.
#define GETBIT(a,i)	( (a) & BITAT(i) ) >> (i)
/// extract masked bits
#define GETBITS(x,i,j) ( ((x) & BITMASK(i,j)) >> (i) )

/// set the #i-th bit of #a to #b
// (b&1) to make sure we get exactly a 0 or 1.
// first mask out the bit there now, then apply ours.
#define SETBIT(a,i,b)	(a) = ((a) & ~BITAT(i)) | (((b) & 1) << (i))




#define FOREACH(i,v) for (typeof((v).begin()) i = (v).begin(); \
                          i != (v).end(); \
                          i++)

#define OPEN_NS namespace pir {
#define CLOSE_NS }

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
    namespace {					\
        int Ctx::StaticInit::num_insts = 0;	\
    }
