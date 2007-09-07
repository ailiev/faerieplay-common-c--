/*
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
