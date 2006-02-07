// -*- c++ -*-

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
// (b&1) to make sure we get exactly a 0 or 1.
// first mask out the bit there now, then apply ours.
#define SETBIT(a,i,b)	(a) = ((a) & ~BITAT(i)) | (((b) & 1) << (i))




#define FOREACH(i,v) for (typeof((v).begin()) i = (v).begin(); \
                          i != (v).end(); \
                          i++)

#define OPEN_NS namespace pir {
#define CLOSE_NS }

#define DECL_STATIC_INIT_WITH_DESTR(initstms,destrstms)			\
    class StaticInit {							\
    public:							        \
	StaticInit () {							\
	    if (num_insts == 0) {					\
                initstms;						\
	    }								\
	    num_insts++;						\
	}								\
        ~StaticInit () {						\
	    if (--num_insts == 0) {					\
		destrstms;						\
	    }								\
	}								\
	static int num_insts;						\
    };

#define DECL_STATIC_INIT(initstms) \
    DECL_STATIC_INIT_WITH_DESTR(initstms, ; )


#define DECL_STATIC_INIT_INSTANCE(Classname)				\
    namespace {								\
        Classname::StaticInit __ ## Classname ## static_init;		\
    }

#define INSTANTIATE_STATIC_INIT(Ctx)					\
    namespace {								\
        int Ctx::StaticInit::num_insts;					\
    }
