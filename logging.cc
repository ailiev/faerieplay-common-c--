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


#include <list>
#include <string>
#include <algorithm>

#include <iostream>

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/BasicConfigurator.hh>

#include "logging.h"
#include "utils.h"


//
// REQUIRES log4cpp versions 0.3.x
// from http://log4cpp.sourceforge.net/
//

using namespace std;

INSTANTIATE_STATIC_INIT(Log);

const char Log::DEFAULT_CONF_FILE_ENV[] = "log4cpp_config_file";

// LOG_MIN_LEVEL should be defined in the Makefile or some config header file.
const Log::log_level Log::min_level = LOG_MIN_LEVEL;


void Log::configure (const std::string& filename)
{
    try {
	log4cpp::PropertyConfigurator::configure (filename);
    }
    catch (const log4cpp::ConfigureFailure& ex) {
	cerr << "Failed to configure log4cpp using " << filename
	     << (getenv(DEFAULT_CONF_FILE_ENV) == NULL ?
		 (string(", and the ") + DEFAULT_CONF_FILE_ENV + " env var is not set.\n") :
		 ".\n")
	     << "Continuing with log4cpp basic configuration." << endl;
	log4cpp::BasicConfigurator::configure();
    }
}


void Log::auto_configure ()
{
    const char* conf_file = getenv (DEFAULT_CONF_FILE_ENV);

    if (conf_file != NULL) {
	configure (conf_file);
    }
    else {
	// a reasonable default
	configure ("log4cpp.properties");
    }
}


Log::logger_t
Log::makeLogger (const string& name,
		 const boost::optional<std::string> & outfile,
		 const boost::optional<Log::log_level>& lev)
{
    // Instantiate the category object
    log4cpp::Category * main_cat =
	& log4cpp::Category::getInstance (name);

    return main_cat;
}



log4cpp::Priority::Value Log::priotrans (Log::log_level l)
{
#define LEV(level) case Log::level: return log4cpp::Priority::level
    switch (l) {
	LEV(CRIT);
	LEV(ERROR);
	LEV(WARN);
	LEV(NOTICE);
	LEV(INFO);
	LEV(DEBUG);
    case Log::PROGRESS: return log4cpp::Priority::INFO;
    case Log::DUMP: return log4cpp::Priority::DEBUG;
    }
#undef LEV

    // not reached I think, but need to satisfy the compiler warning.
    return log4cpp::Priority::INFO;
}
