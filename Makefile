include ../common.make

SRCS=sym_crypto.cc sym_crypto_mac.cc sym_crypto_hash.cc \
	record_x_xdr.c record.cc \
	utils.cc consts.cc hash.cc comm_types_xdr.c hostcall.cc \
	sccutils.c socket-class.cc openssl_crypto.cc xdr_class.cc \
	udp-socket.cc scc-socket.cc


_ccobjs=$(SRCS:.cc=.o)
OBJS=$(_ccobjs:.c=.o)

OBJS_crypto=sym_crypto.o sym_crypto_mac.o sym_crypto_hash.o \
	utils.o openssl_crypto.o
OBJS_record=record_x_xdr.o record.o 

ifdef HAVE_OPENSSL
LDLIBFILES = -lssl -lcrypto
endif

TARGETS=libcommon.a record sym_crypto

all: libcommon.$(LIBEXT)

install: $(LEEDS_LIB)/libcommon.so

$(LEEDS_LIB)/libcommon.so: libcommon.so
	install $^ $(LEEDS_LIB)

testudp : CPPFLAGS += -D_TESTING_UDP_SOCK
testudp: udp-socket.o
	$(CXXLINK)

xdr_class : CPPFLAGS += -D_TESTING_XDR_CLASS
xdr_class: xdr_class.o
	$(CXXLINK)


libcommon.so: $(OBJS)
	$(CXXLINK)

libcommon.a: $(OBJS)
	ar -ru $@ $^
# do a "fake" link to get the template instantiations into the .o files (goes
# with using g++ -frepo)
#	$(CXX) $(LDFLAGS) $^ -o fake-libs.exe || true


sccutils.o : CPPFLAGS += -I$(TOP)/$(TREE)/include
scc-socket.o : CPPFLAGS += -I$(TOP)/$(TREE)/include


symcrypto : CPPFLAGS += -DTESTING_SYM_CRYPTO
symcrypto : LDLIBFILES += -lcommon
symcrypto : LIBDIRS += .
symcrypto: sym_crypto.o
	$(CXXLINK)

record : CPPFLAGS += -D_TESTING_RECORD
record: $(OBJS_record)
	$(CXXLINK)

hash: hash.o ../host/libhost.a
	$(CXXLINK)

utils-test : CPPFLAGS += -D_TESTING_UTILS_CC
utils-test: utils.o consts.o
	$(CXXLINK)




include ../footer.make
