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

# define TOOLS_DIR if need to use some of the build tools and libraries
# from a seprate directory.
# TOOLS_DIR=$(this_dir)/../../tools
TOOLS_DIR=$(HOME)/faerieplay/tools

# TTP code should be be built for 4758, or normal linux host?
# (set if not already set, eg, on cmd line.)
# TTP_TARGET ?= 4758
TTP_TARGET ?= linux

# directories of include files added to all C/C++ compilations.
EXTRA_INCLUDE_DIRS += $(TOOLS_DIR)/usr/include
# EXTRA_INCLUDE_DIRS += /usr/local/include

# directories with extra libraries
EXTRA_LIB_DIRS = $(TOOLS_DIR)/usr/lib


##############################
# compilers
##############################
# C++
CXX:=g++
# C
CC:=gcc

CXXFLAGS += -O2
CFLAGS += -O2

##############################
# 4758 headers locations. Only needed if compiling the Circuit VM for the 4758.
##############################
TOP_4758  = $(HOME)/work/4758
TREE_4758 = 4758


##############################
# Is this build for running the TTP components on a 4758, or on a normal host?
# Comment out if compiling for host-only operation.
##############################
# RUNONCARD := 1


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
