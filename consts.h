/* -*-c++-*- */

/*
Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu>

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of the author nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

This product includes cryptographic software written by Eric Young
(eay@cryptsoft.com)

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
