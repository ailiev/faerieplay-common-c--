/*
 * Library routines for the Dartmouth CS Private Information Retrieval and
 * Writing (PIR/W) library.
 *
 * Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu>
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
 * common/consts.cc
 * alex iliev, jan 2003
 * constants used on host and card
 */

#include "consts.h"

#include <string>
#include <stdlib.h>

// the default storage directory on the host
const std::string STOREROOT = "pirw_default_store_root";

const unsigned short PIR_SEARCH_PORT = 1054,
    PIR_CONTROL_PORT  = 1053,
    PIR_HOSTSERV_PORT = 1030;


const size_t BUFSIZE = 128;	// this is a starting buf size, make it small

const std::string DIRSEP = "/";

// for the purpose of SCC sockets, the host has number 0, and the cards go from
// 1
const unsigned short CARDNO_RETRIEVER = 2,
    CARDNO_PERMUTER = 1;

//const size_t CACHEMEM = 1<<10;
