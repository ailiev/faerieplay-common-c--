include ../common.make

SRCS=sym_crypto.cc cbcmac.c sym_crypto_mac.cc record_x_xdr.c record.cc \
	utils.cc

_CCOBJS=$(SRCS:.cc=.o)
OBJS=$(_CCOBJS:.c=.o)

OBJS_crypto=cbcmac.o sym_crypto.o sym_crypto_mac.o utils.o
OBJS_record=record_x_xdr.o record.o 

LDFLAGS=-lssl -lcrypto

# CPPFLAGS += -D_TESTING_RECORD

TARGETS=libcommon.a record sym_crypto
all: $(TARGETS)


%_xdr.c %.h: %.x
	rpcgen $<



libcommon.so: $(OBJS)
	$(CXX) -shared $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

libcommon.a: $(OBJS)
	ar -ru $@ $^




sym_crypto: $(OBJS_crypto)
	$(CXX) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

record: $(OBJS_record)
	$(CXX) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@


depend:
	$(CXX) -M $(CPPFLAGS) $(SRCS) > $@


# the dependencies
include depend
