/* -*-c++-*-
 * ** PIR Private Directory Service prototype
 * ** Copyright (C) 2002 Alexander Iliev <iliev@nimbus.dartmouth.edu>
 * **
 * ** This program is free software; you can redistribute it and/or modify
 * ** it under the terms of the GNU General Public License as published by
 * ** the Free Software Foundation; either version 2 of the License, or
 * ** (at your option) any later version.
 * **
 * ** This program is distributed in the hope that it will be useful,
 * ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 * ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * ** GNU General Public License for more details.
 * **
 * ** You should have received a copy of the GNU General Public License
 * ** along with this program; if not, write to the Free Software
 * ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * */

/*
 * common/consts.h
 * alex iliev, jan 2003
 * constants used on host and card
 */


#ifndef _HOST_CONSTS_H
#define _HOST_CONSTS_H


#include <string>

#include <stdlib.h>

extern const char* STOREROOT;


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


#endif // _HOST_CONSTS_H
