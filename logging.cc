#include <list>
#include <string>
#include <algorithm>

#include <iostream>

#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/SimpleLayout.hh>

#include "logging.h"
#include <common/utils.h>


//
// REQUIRES log4cpp versions 0.3.x
// from http://log4cpp.sourceforge.net/
//

using namespace std;


// MAX_LOG_LEVEL should be defined in the Makefile or some config header file.
const Log::log_level Log::min_level = LOG_MIN_LEVEL;


unsigned Log::next_idx = 0;






#if 0

// have abandoned this "enabled modules" stuff for now, but could still be
// useful.

#ifndef LOG_ENABLED_MODULES
#define LOG_ENABLED_MODULES {"run-circuit", "array", "batcher-permute", "batcher-network"}
#endif


namespace {
    vector<string> * setup_mask ();
}


uint32_t Log::log_enabled_modules_mask = 0;

bool Log::inited_enabled_names = false;

vector<string> * Log::enabled_names;

unsigned Log::add_module (const std::string& name)
{
    if (!inited_enabled_names) {
	enabled_names = setup_mask ();
	inited_enabled_names = true;
    }

    unsigned answer = ++next_idx;

    assert (answer <= 31);
    
    if (elem (name, *enabled_names)) {
	SETBIT (log_enabled_modules_mask, answer, 1);
    }

    return answer;
}


vector<string> * setup_mask ()
{
    // the enabled names should be in LOG_ENABLED_MODULES:
    string modules[] = LOG_ENABLED_MODULES;

    vector<string> * answer = new vector<string> (ARRLEN(modules));

    copy (modules, modules + ARRLEN(modules),
	  answer->begin());

    return answer;
}
#endif // 0




Log::logger_t
Log::makeLogger (const string& name,
		 const boost::optional<std::string> & outfile,
		 const boost::optional<Log::log_level>& lev)
{
    // adapted from http://developers.sun.com/solaris/articles/logging.html
    
    // 1 instantiate an appender object that 
    // will append to a log file or to clog
    log4cpp::Appender* app = outfile	?
	static_cast<log4cpp::Appender*> (
	    new log4cpp::FileAppender (name,
				       *outfile))	          :
	static_cast<log4cpp::Appender*> (
	    new log4cpp::OstreamAppender (name, &std::clog));


    // 2. Instantiate a layout object
    // Two layouts come already available in log4cpp
    // unless you create your own.
    // BasicLayout includes a time stamp
    log4cpp::Layout* layout = 
        new log4cpp::SimpleLayout();

    // 3. attach the layout object to the 
    // appender object
    app->setLayout(layout);

    // 4. Instantiate the category object
    // the priority is the max prio which this object will log, and lower
    // (noisier) log messages will be ignored.
    log4cpp::Category * main_cat =
	& log4cpp::Category::getInstance (name);

    // 5. Step 1 
    // an Appender when added to a category becomes
    // an additional output destination unless 
    // Additivity is set to false when it is false,
    // the appender added to the category replaces
    // all previously existing appenders
    main_cat->setAdditivity(false);

    // 5. Step 2
    // this appender becomes the only one
    main_cat->setAppender(app);

    main_cat->setPriority (lev ? priotrans(*lev) : priotrans (Log::min_level));


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

void Log::show_log (unsigned level, logger_t logger)
{
//     std::clog << "Log req at level " << level
// 	      << ", with min level " << Log::min_level
// 	      << std::endl;
}
