include ../common.make

SRCS=sym_crypto.cc cbcmac.c sym_crypto_mac.cc record_x_xdr.c record.cc \
	utils.cc consts.cc hash.cc comm_types_xdr.c hostcall.cc \
	sccutils.c socket-class.cc

_CCOBJS=$(SRCS:.cc=.o)
OBJS=$(_CCOBJS:.c=.o)

OBJS_crypto=cbcmac.o sym_crypto.o sym_crypto_mac.o utils.o
OBJS_record=record_x_xdr.o record.o 

LDLIBS=-lcommon -L. -lssl -lcrypto


TARGETS=libcommon.a record sym_crypto

all: libcommon.a


%_xdr.c %.h: %.x
	rpcgen $<



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




include ../footer.make
