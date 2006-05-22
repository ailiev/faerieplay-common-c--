include ../utils.make
include ../header.make


LIBSRCS=sym_crypto.cc sym_crypto_mac.cc sym_crypto_hash.cc \
	utils.cc consts.cc comm_types_xdr.c hostcall.cc \
	socket-class.cc xdr_class.cc logging.cc bytebuffer.cc

TESTSRCS = $(wildcard test-*.cc)



ifdef HAVE_OPENSSL
LDLIBFILES  += -lssl -lcrypto
LIBSRCS	    +=  openssl_crypto.cc
endif

ifdef COMM_SCC
LIBSRCS += sccutils.c scc-socket.cc
endif

ifdef COMM_UDP
LIBSRCS += udp-socket.cc
endif

SRCS=$(LIBSRCS)

TARGETS=libcommon.$(LIBEXT)

all: $(TARGETS)

install: $(TARGETS)
	$(INSTALL) $^ $(LEEDS_LIB)


#$(LEEDS_LIB)/libcommon.so: libcommon.so
#	$(INSTALL) $^ $(LEEDS_LIB)

udp-socket-main.o : CPPFLAGS += -DTESTING_UDP_SOCK
udp-socket-main.o: udp-socket.cc
	$(CXXCOMP)
testudp : LIBDIRS    += $(LEEDS_LIB)
testudp : LDLIBFILES += -lcommon
testudp: udp-socket-main.o $(LDLIBFILES)
	$(CXXLINK)


libcommon.so: $(LIBOBJS)
	$(CXXLINK)

libcommon.a: $(LIBOBJS)
	ar -ru $@ $^
# do a "fake" link to get the template instantiations into the .o files (goes
# with using g++ -frepo)
#	$(CXX) $(LDFLAGS) $^ -o fake-libs.exe || true


#sccutils.o : CPPFLAGS += -I$(TOP)/$(TREE)/include
#scc-socket.o : CPPFLAGS += -I$(TOP)/$(TREE)/include


# symcrypto test
#name:=sym_crypto
#mainmacro:=TESTING_SYM_CRYPTO
#include ../generate-main-rule.make
#sym_crypto-main : LDLIBFILES	+= -lcommon -lcard
#sym_crypto-main : LIBDIRS	+= . $(LEEDS_LIB)

#name:=xdr_class
#mainmacro:=TESTING_XDR_CLASS
#include ../generate-main-rule.make


tests: $(TESTEXES)

$(TESTEXES): $(LIBOBJS)


utils-test : CPPFLAGS += -DTESTING_UTILS_CC
utils-test: utils.o consts.o
	$(CXXLINK)




include ../footer.make
