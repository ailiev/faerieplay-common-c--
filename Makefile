include ../common.make

SRCS=sym_crypto.cc cbcmac.c sym_crypto_mac.cc sym_crypto_hash.cc \
	record_x_xdr.c record.cc \
	utils.cc consts.cc hash.cc comm_types_xdr.c hostcall.cc \
	sccutils.c socket-class.cc openssl_crypto.cc xdr_class.cc

_CCOBJS=$(SRCS:.cc=.o)
OBJS=$(_CCOBJS:.c=.o)

OBJS_crypto=cbcmac.o sym_crypto.o sym_crypto_mac.o sym_crypto_hash.o \
	utils.o openssl_crypto.o
OBJS_record=record_x_xdr.o record.o 

LDLIBS=-lcommon -L. -lssl -lcrypto #-L/home/sasho/minime/ssl/lib


TARGETS=libcommon.a record sym_crypto

# .INTERMEDIATE: record_x_xdr.c comm_types_xdr.c

all: libcommon.a


%_xdr.c %.h: %.x
	rpcgen $<


xdr_class : CPPFLAGS += -D_TESTING_XDR_CLASS
xdr_class: xdr_class.o
	$(CXXLINK)


libcommon.so: $(OBJS)
	$(CXX) -shared $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

libcommon.a: $(OBJS)
	ar -ru $@ $^


sccutils.o : CPPFLAGS += -I$(TOP)/$(TREE)/include
socket-class.o : CPPFLAGS += -I$(TOP)/$(TREE)/include


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
