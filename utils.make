# -*- makefile -*-

INSTALL=cp -up


OBJS=$(call mkobjs,$(SRCS))
LIBOBJS=$(call mkobjs,$(LIBSRCS))

TESTOBJS=$(call mkobjs,$(TESTSRCS))
TESTEXES=$(TESTOBJS:.o=)


mkobjs = $(patsubst %.c,%.o,$(patsubst %.cc,%.o,$(patsubst %.C,%.o,$(1))))

# the link command with C++ linker
# get rid of .so and .a files mentioned in the prereqs (probably expanded from -libX
# prereqs by make)
CXXLINK_ARGS=$(LDFLAGS) $(filter-out %.so %.a,$^) $(LOADLIBES) $(LDLIBS) -o $@
CXXLINK = $(CXX) $(CXXLINK_ARGS)

CXXCOMP=$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

AR_CMD=ar -ru $@ $^


# to build a shared object is like linking with -shared flag, so can just use
# the CXXLINK command
%.so : LDFLAGS += -shared

lib%.so: %.o $(LIBOBJS)
	$(CXXLINK)

# set up library paths and dependencies, based on vars LIBDIRS and LDLIBFILES
LDLIBS += $(patsubst %,-L%,$(LIBDIRS)) \
	  $(LDLIBFILES)


# some output dir name conventions
DIST_BIN = $(DIST_ROOT)/bin
DIST_LIB = $(DIST_ROOT)/lib

%.h: %.x
	rpcgen $<

%_xdr.c: %.x
	rpcgen $<

# add the produce of rpcgen to what can be deleted
TRASH += $(patsubst %.x,%_xdr.c,$(wildcard *.x)) $(patsubst %.x,%.h,$(wildcard *.x))
