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


#include "logging.h"

Log::logger_t logger = Log::makeLogger ("pir.common.test-logging");

int main (int argc, char * argv[])
{
//    Log::configure ("log4cpp.properties");
    
    LOG (Log::ERROR, logger, "An error " << 1234);
    LOG (Log::WARN, logger, "An warn " << 1234);
    LOG (Log::INFO, logger, "An info " << 1234);
    LOG (Log::DEBUG, logger, "An debug " << 1234);
}
