// -*- c++ -*-
//#include <boost/preprocessor/facilities/expand.hpp>
#include <vector>
#include <string>

#include "utils-macros.h"

#include <iostream>

#ifndef _LOGGING_H
#define _LOGGING_H


// should set this in the Makefile best
#ifndef LOG_MAX_LEVEL
#define LOG_MAX_LEVEL		Log::PROGRESS
#endif




class Log {
public:
    
    // low value has higher priority
    enum log_levels {
	CRIT = 0,
	ERR,
	WARNING,
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

    static unsigned next_idx;

    static bool inited_enabled_names;

    static std::vector<std::string> * enabled_names;

};


// the compiler should be able to remove the true branch if the
// level condition fails (??)
#define LOG(level,modules,msg) \
    if (level < LOG_MAX_LEVEL && \
        BITAT(modules) & Log::log_enabled_modules_mask != 0 ) \
    {\
         std::clog << msg << std::endl; \
    }


#endif // _LOGGING_H
