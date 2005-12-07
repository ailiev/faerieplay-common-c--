include ../common.make

SRCS=sym_crypto.cc cbcmac.c sym_crypto_mac.cc sym_crypto_hash.cc \
	record_x_xdr.c record.cc \
	utils.cc consts.cc hash.cc comm_types_xdr.c hostcall.cc \
	sccutils.c socket-class.cc openssl_crypto.cc xdr_class.cc \
	udp-socket.cc scc-socket.cc


_ccobjs=$(SRCS:.cc=.o)
OBJS=$(_ccobjs:.c=.o)

OBJS_crypto=cbcmac.o sym_crypto.o sym_crypto_mac.o sym_crypto_hash.o \
	utils.o openssl_crypto.o
OBJS_record=record_x_xdr.o record.o 

LDLIBS=-L. -lcommon -lssl -lcrypto #-L/home/sasho/minime/ssl/lib


TARGETS=libcommon.a record sym_crypto

# .INTERMEDIATE: record_x_xdr.c comm_types_xdr.c

all: libcommon.so

install: libcommon.so
	install $^ $(HOME)/leeds_root

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


sym_crypto: $(OBJS_crypto)
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
