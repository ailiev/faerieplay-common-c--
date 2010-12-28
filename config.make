# -*- makefile -*-

# this reading of MAKEFILE_LIST has to be done before any other files are
# include-d!
this_file := $(lastword $(MAKEFILE_LIST))
this_dir := $(dir $(realpath $(this_file)))


##############################
# the main output directory, for binaries and libraries.
# cannot be a relative directory, as it is referenced from many makefiles in
# different directories.
##############################
DIST_ROOT=$(HOME)/faerieplay

##############################
# compilers
##############################
# C++
CXX:=g++
# C
CC:=gcc

CXXFLAGS += -O2
CFLAGS += -O2


#
# Some derived vars, no need to touch
#
tools_dir_real = $(realpath $(TOOLS_DIR))


#
# checks
#
ifeq ($(CXX), )
error (No C++ compiler specified with CXX)
endif

ifeq ($(CC), )
error (No C compiler specified with CC)
endif
