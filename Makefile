PIR_DIR=.

include $(PIR_DIR)/utils.make
include $(PIR_DIR)/header.make


LIBSRCS=utils.cc \
	socket-class.cc logging.cc bytebuffer.cc \
	exceptions.cc

HEADERS=$(wildcard *.h) $(wildcard *.hpp)

TESTSRCS = $(wildcard test-*.cc)



#ifdef HAVE_OPENSSL
#LDLIBFILES  += -lssl -lcrypto
#LIBSRCS	    +=  openssl_crypto.cc
#endif

ifdef COMM_UDP
LIBSRCS += udp-socket.cc
endif

SRCS=$(LIBSRCS)

TARGETS=libcommon.$(LIBEXT)

logging.o : CPPFLAGS += -DLOG_MIN_LEVEL=$(LOG_MIN_LEVEL)

all: $(TARGETS)

install: $(TARGETS) | init
	$(INSTALL) $^ $(DIST_LIB)/
	mkdir -p $(DIST_ROOT)/include/pir/common
	$(INSTALL) $(HEADERS) $(DIST_ROOT)/include/pir/common/

libcommon.so: $(LIBOBJS)
	$(CXXLINK)

libcommon.a: $(LIBOBJS)
	$(AR_CMD)
# do a "fake" link to get the template instantiations into the .o files (goes
# with using g++ -frepo)
#	$(CXX) $(LDFLAGS) $^ -o fake-libs.exe || true

tests: $(TESTEXES)

$(TESTEXES): $(LIBOBJS)



include footer.make
