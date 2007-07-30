PIR_DIR=..

include $(PIR_DIR)/utils.make
include $(PIR_DIR)/header.make


LIBSRCS=sym_crypto.cc sym_crypto_mac.cc sym_crypto_hash.cc \
	utils.cc consts.cc \
	comm_types_xdr.c comm_types_utils.c \
	hostcall.cc \
	socket-class.cc xdr_class.cc logging.cc bytebuffer.cc \
	exceptions.cc

TESTSRCS = $(wildcard test-*.cc)



#ifdef HAVE_OPENSSL
#LDLIBFILES  += -lssl -lcrypto
#LIBSRCS	    +=  openssl_crypto.cc
#endif

ifdef COMM_SCC
LIBSRCS += sccutils.c scc-socket.cc
endif

ifdef COMM_UDP
LIBSRCS += udp-socket.cc
endif

SRCS=$(LIBSRCS)

TARGETS=libcommon.$(LIBEXT)

logging.o : CPPFLAGS += -DLOG_MIN_LEVEL=$(LOG_MIN_LEVEL)

all: $(TARGETS)

install: $(TARGETS)
	$(INSTALL) $^ $(LEEDS_LIB)


libcommon.so: $(LIBOBJS)
	$(CXXLINK)

libcommon.a: $(LIBOBJS)
	$(AR_CMD)
# do a "fake" link to get the template instantiations into the .o files (goes
# with using g++ -frepo)
#	$(CXX) $(LDFLAGS) $^ -o fake-libs.exe || true


#sccutils.o : CPPFLAGS += -I$(TOP)/$(TREE)/include
#scc-socket.o : CPPFLAGS += -I$(TOP)/$(TREE)/include


tests: $(TESTEXES)

$(TESTEXES): $(LIBOBJS)



include ../footer.make
