PIR_DIR=.

include $(PIR_DIR)/utils.make
include $(PIR_DIR)/header.make


LIBSRCS=utils.cc \
	socket-class.cc logging.cc bytebuffer.cc \
	exceptions.cc

HEADERS=$(wildcard *.h) $(wildcard *.hpp)

MAKE_INCLUDES=config.make footer.make header.make shared-targets.make utils.make

TESTSRCS = $(wildcard test-*.cc)



#ifdef HAVE_OPENSSL
#LDLIBFILES  += -lssl -lcrypto
#LIBSRCS	    +=  openssl_crypto.cc
#endif

ifdef COMM_UDP
LIBSRCS += udp-socket.cc
endif

SRCS=$(LIBSRCS)

TARGETS=libfaerieplay-common.$(LIBEXT)

logging.o : CPPFLAGS += -DLOG_MIN_LEVEL=$(LOG_MIN_LEVEL)

all: $(TARGETS)

install: $(TARGETS) | init
	$(INSTALL) $^ $(DIST_LIB)/
	mkdir -p $(DIST_ROOT)/include/faerieplay/common
	$(INSTALL) $(HEADERS) $(DIST_ROOT)/include/faerieplay/common/
	mkdir -p $(DIST_ROOT)/make_include/faerieplay/common
	$(INSTALL) $(MAKE_INCLUDES) $(DIST_ROOT)/make_include/faerieplay/common/

libfaerieplay-common.so: $(LIBOBJS)
	$(CXXLINK)

libfaerieplay-common.a: $(LIBOBJS)
	$(AR_CMD)
# do a "fake" link to get the template instantiations into the .o files (goes
# with using g++ -frepo)
#	$(CXX) $(LDFLAGS) $^ -o fake-libs.exe || true

tests: $(TESTEXES)

$(TESTEXES): $(LIBOBJS)



include footer.make
