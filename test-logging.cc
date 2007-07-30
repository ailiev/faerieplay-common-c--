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
