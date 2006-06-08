// -*- c++ -*-
//#include <boost/preprocessor/facilities/expand.hpp>
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


// should set this in the Makefile best
#ifndef LOG_MAX_LEVEL
#define LOG_MAX_LEVEL		Log::DUMP
#endif




class Log {
public:
    
    // low value has higher priority
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

    /// adds a new module for logging purposes, with the given name.
    /// @return the logging index for this module, which it should use in
    /// the LOG macro.
    ///
    /// For now this should be done on a per-file basis, as there will be
    /// only a few indices available.
    static unsigned add_module (const std::string& name);

    static uint32_t log_enabled_modules_mask;

    // if we're doing runtime level checking, use this instead of LOG_MAX_LEVEL
    // macro
    static const log_level max_level;

    static unsigned next_idx;

    static bool inited_enabled_names;

    static std::vector<std::string> * enabled_names;

    typedef log4cpp::Category * logger_t;

    static void show_log (unsigned level, logger_t logger);

    static logger_t
    makeLogger (const std::string& name,
		const boost::optional<std::string> & outfile = boost::none,
		const boost::optional<log_level>& lev = boost::none);

    static log4cpp::Priority::Value priotrans (Log::log_level l);
};



// the compiler should be able to remove the true branch if the
// level condition fails (??)
#define LOG(level,logger,msg) LOGNOLN(level,logger,msg << LOG_ENDENTRY)

#define LOGNOLN(level,logger,msg)			\
if (level <= Log::max_level ||				\
    logger->getPriority() <= Log::priotrans (level))	\
{							\
    Log::show_log (level, logger);			\
    logger->getStream (Log::priotrans (level))		\
	<< std::setw (18) << logger->getName() << ": " << msg;		\
}

#define LOG_ENDL "\n"
#define LOG_ENDENTRY log4cpp::CategoryStream::ENDLINE

#endif // _LOGGING_H
