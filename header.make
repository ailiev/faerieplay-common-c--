# -*-makefile-gmake-*-

# this reading of MAKEFILE_LIST has to be done before any other files are
# include-d!
this_file := $(lastword $(MAKEFILE_LIST))
this_dir := $(dir $(realpath $(this_file)))

include $(this_dir)/config.make


ifeq ($(TTP_TARGET), 4758)
	HAVE_4758_CRYPTO := 1
	COMM_SCC := 1
	LINKING := static
	CFLAGS += -O2

	LOG_MIN_LEVEL := Log::ERROR
else ifeq ($(TTP_TARGET), linux)
	HAVE_OPENSSL := 1
	COMM_UDP := 1
	LINKING := dynamic
	CFLAGS += -ggdb3

	LOG_MIN_LEVEL := Log::DEBUG
else
$(error Could not recognize TTP_TARGET='$(TTP_TARGET)'. Should be '4758' or 'linux')
endif


# pick up any extra include dirs specified on command line or in config.make
ifdef EXTRA_INCLUDE_DIRS
	CPPFLAGS += $(patsubst %,-I%,$(EXTRA_INCLUDE_DIRS))
endif


ifdef EXTRA_LIB_DIRS
	LIBDIRS += $(EXTRA_LIB_DIRS)
	vpath %.a $(EXTRA_LIB_DIRS)
endif


CXXFLAGS = $(CFLAGS)

CPPFLAGS += -I.. -I.
CPPFLAGS += 

CFLAGS += -Wall
# CFLAGS += -pedantic

#CFLAGS += -pg
#CFLAGS += -fprofile-arcs
#LDFLAGS += -pg
#CFLAGS += -O2
#CFLAGS += -ansi
#CXXFLAGS += -fno-default-inline


ifeq "$(LINKING)" "static"
	.LIBPATTERNS := lib%.a lib%.so
	LIBEXT := a
	LDFLAGS += -static
endif
ifeq "$(LINKING)" "dynamic"
	.LIBPATTERNS := lib%.so lib%.a
	LIBEXT := so
endif


# -rdynamic has the effect of including lots more symbols in the binary image
#LDFLAGS=-rdynamic # -pg

# -frepo: not really worth it for now, negligible space savings
# CXXFLAGS += -frepo

# make all components look in common/ for library files
#vpath %.a ../common

#LDLIBS+=-L/usr/lib/debug

LDLIBS += -L$(TOOLS_DIR)/usr/lib

ifdef COMM_UDP
	CPPFLAGS += -DHOSTCALL_COMM_UDP
endif
ifdef COMM_SCC
	CPPFLAGS += -DHOSTCALL_COMM_SCC
	CPPFLAGS += -I$(TOP_4758)/$(TREE_4758)/include
endif


# CPPFLAGS += -DNDEBUG

# CPPFLAGS += -DVALGRIND


OUTROOT=objs/debug
OUTDIR=$(OUTROOT)/$(THISDIR)

LEEDS_BIN=$(LEEDS_ROOT)/bin
LEEDS_LIB=$(LEEDS_ROOT)/lib


# define some sensible names instead of LEEDS_ROOT etc.
# users of this makefile can override.
LEEDS_ROOT = $(DIST_ROOT)

.DEFAULT_GOAL := all

init::
	mkdir -p $(DIST_BIN) $(DIST_LIB)

.PHONY: init
