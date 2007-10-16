// -*- c++ -*-
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

#include <vector>
#include <string>

#include <iostream>
#include <iomanip>

#include <stdint.h>

#include <boost/optional/optional.hpp>
#include <boost/none.hpp>

#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>

#include "utils-macros.h"


#ifndef _LOGGING_H
#define _LOGGING_H


//
// REQUIRES log4cpp versions 0.3.x
// from http://log4cpp.sourceforge.net/
// or Debian package liblog4cpp4-dev
//


// should set this in the Makefile best
#ifndef LOG_MIN_LEVEL
#define LOG_MIN_LEVEL		Log::DUMP
#endif




class Log {
public:
    
    // low value has higher priority
    // FIXME: this appears to be the opposite of how log4cpp does it, more
    // chance of confusion.
    enum log_level {
	CRIT = 0,
	ERROR,
	WARN,
	NOTICE,
	INFO,
	PROGRESS,
	DEBUG,
	// really dump everything!
	DUMP

    };

    typedef log4cpp::Category * logger_t;

    static void configure (const std::string& filename);

    static void auto_configure ();

    ///
    /// Environment variable consulted for the location of a log4cpp config file
    /// (in the (old) log4j properties format)
    static const char DEFAULT_CONF_FILE_ENV[];
    
    /// Create a new logger.
    /// 
    /// @param lev set the priority of this logger, ie. the minimum (most
    /// verbose) priority of messages to it that will be displayed. Messages
    /// with lower priority will not be displayed, unless Log::min_level allows
    /// them through.
    static logger_t
    makeLogger (const std::string& name,
		const boost::optional<std::string> & outfile = boost::none,
		const boost::optional<log_level>& lev = boost::none);




    static log4cpp::Priority::Value priotrans (Log::log_level l);

    /// If any log message is higher or equal priority as min_level, it will be
    /// considered further for display.
    /// See also the lev parameter to makeLogger().
    static const log_level min_level;

    DECL_STATIC_INIT (
	auto_configure()
	);
};


DECL_STATIC_INIT_INSTANCE(Log);


#define LOG(level,logger,msg) LOGNOLN(level,logger,msg << LOG_ENDENTRY)

#define LOGNOLN(level,logger,msg)			\
if (level <= Log::min_level ||				\
    logger->getPriority() <= Log::priotrans (level))	\
{							\
    logger->getStream (Log::priotrans (level)) << msg;  \
}

#define LOG_ENDL "\n"
#define LOG_ENDENTRY log4cpp::CategoryStream::ENDLINE

#endif // _LOGGING_H
