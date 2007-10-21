/* -*-c++-*- */
/*
 * Library routines for the Dartmouth CS Private Information Retrieval and
 * Writing (PIR/W) library.
 *
 * Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu> and
 * Sean W. Smith <sws@cs.dartmouth.edu>
 *
 * All rights reserved.
 *
 * This code is released under a BSD license.
 * Please see LICENSE.txt for the full license and disclaimers.
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com)
*/

/*
 * common/consts.h
 * alex iliev, jan 2003
 * constants used on host and card
 */


#ifndef _HOST_CONSTS_H
#define _HOST_CONSTS_H


#include <string>

#include <stdlib.h>

extern const std::string STOREROOT;


extern const unsigned short PIR_SEARCH_PORT, // where the host sends requests to
                                             // the card
    
    PIR_CONTROL_PORT, // where the retriever listens for the shuffler's new DB
                      // notices
    
    PIR_HOSTSERV_PORT; // where the card sends service requests to the host



extern const std::string DIRSEP; // the directory separator
				// FIXME: is it really common to the host and
				// card?

extern const size_t  BUFSIZE;

// the card numbers of the retrieval card and the shuffling card
extern const unsigned short CARDNO_RETRIEVER, CARDNO_PERMUTER;


static const size_t DEFAULT_MAX_OBJ_SIZE = 16 * 1024;

// how many bytes to give to each cache.
// FIXME: what really matters is the total cache memory used!
const size_t CACHEMEM = 1<<10;


#endif // _HOST_CONSTS_H
