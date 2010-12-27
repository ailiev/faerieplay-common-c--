# -*-makefile-*-


# make sure to link test objects with C++, as by default make wants to use gcc
# PROBLEM: this causes all dependency compiles to use C++ too, even if it is a
# plain c file.
$(TESTEXES) : CC=$(CXX)


# putting here so it's after all the internal libraries in the command line.
# FIXME: should not force people to link with this, eg. does not make sense for
# the C code targeting MIPS.
LDLIBS += -llog4cpp


$(TESTEXES): $(LIBOBJS)

tests: $(TESTEXES)

ifdef PREDEPCMD
predep := $(PREDEPCMD)
else
predep := true
endif

#depend : CPPFLAGS += -I$(TOP)/$(TREE)/include
dep: $(SRCS) $(LIBSRCS) $(TESTSRCS)
	$(predep)
	$(CXX) -MM $(CPPFLAGS) $^ > depend

-include depend

# default if noone else sets it.
CLEAN_HOOK ?=

clean:
	$(RM) $(TARGETS) $(OBJS) $(LIBOBJS) $(EXES) $(TESTOBJS) $(TESTEXES) $(TRASH)
	$(CLEAN_HOOK)
