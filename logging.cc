#include <list>
#include <string>
#include <algorithm>

#include "logging.h"
#include <common/utils.h>

#ifndef LOG_ENABLED_MODULES
#define LOG_ENABLED_MODULES {"run-circuit"}
#endif



using namespace std;


uint32_t Log::log_enabled_modules_mask = 0;


unsigned Log::next_idx = 0;

bool Log::inited_enabled_names = false;

vector<string> * Log::enabled_names;



static vector<string> * setup_mask ();


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

