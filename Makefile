include ../common.make

LIBSRCS=sym_crypto.cc sym_crypto_mac.cc sym_crypto_hash.cc \
	utils.cc consts.cc comm_types_xdr.c hostcall.cc \
	socket-class.cc xdr_class.cc logging.cc bytebuffer.cc

TESTSRCS=template-test.cc
TESTOBJS=$(call mkobjs,$(TESTSRCS))

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

tests: $(TESTOBJS)


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
symcrypto : LDLIBFILES	+= -lcommon -lcard
symcrypto : LIBDIRS	+= . $(LEEDS_LIB)
symcrypto: sym_crypto_main.o $(LDLIBFILES)
	$(CXXLINK)
sym_crypto_main.o: sym_crypto.cc
	$(CXXCOMP)
sym_crypto_main.o : CPPFLAGS += -DTESTING_SYM_CRYPTO


xdrtest: xdr_class-main.o consts.o comm_types_xdr.o logging.o bytebuffer.o
	$(CXXLINK)
xdr_class-main.o : CPPFLAGS += -DTESTING_XDR_CLASS
xdr_class-main.o: xdr_class.cc
	$(CXXCOMP)


utils-test : CPPFLAGS += -DTESTING_UTILS_CC
utils-test: utils.o consts.o
	$(CXXLINK)




include ../footer.make
