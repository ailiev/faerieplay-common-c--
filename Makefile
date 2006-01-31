include ../common.make

LIBSRCS=sym_crypto.cc sym_crypto_mac.cc sym_crypto_hash.cc \
	utils.cc consts.cc hash.cc comm_types_xdr.c hostcall.cc \
	socket-class.cc xdr_class.cc


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
	install $^ $(LEEDS_LIB)

#$(LEEDS_LIB)/libcommon.so: libcommon.so
#	install $^ $(LEEDS_LIB)

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



utils-test : CPPFLAGS += -DTESTING_UTILS_CC
utils-test: utils.o consts.o
	$(CXXLINK)




include ../footer.make
