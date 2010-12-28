# -*-makefile-gmake-*-

# this reading of MAKEFILE_LIST has to be done before any other files are
# include-d!
this_file := $(lastword $(MAKEFILE_LIST))
this_dir := $(dir $(realpath $(this_file)))

include $(this_dir)/config.make


LINKING := dynamic
LOG_MIN_LEVEL := Log::DEBUG
COMM_UDP := 1
CFLAGS += -ggdb3

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

# CPPFLAGS += -DNDEBUG

# CPPFLAGS += -DVALGRIND


OUTROOT=objs/debug
OUTDIR=$(OUTROOT)/$(THISDIR)

.DEFAULT_GOAL := all

init::
	mkdir -p $(DIST_BIN) $(DIST_LIB)

.PHONY: init
