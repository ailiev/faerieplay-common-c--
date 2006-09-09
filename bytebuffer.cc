#include "countarray.hpp"

#ifndef NDEBUG
#include <ios>
#endif


Log::logger_t   CountedByteArray::logger;


INSTANTIATE_STATIC_INIT(CountedByteArray);


std::ostream& operator<< (std::ostream& os, const CountedByteArray& buf)
{
    const byte * bytes = buf.data();
    
    // this doesn't produce hex for some reason
//    std::ios::fmtflags fl = os.setf (std::ios::hex);

//    os << "{";

    for (unsigned i=0; i < buf.len(); i++)
    {
	os << static_cast<unsigned>(bytes[i]);
	if (i+1 < buf.len()) {
	    os << ",";
	}
    }

//    os << "}";

//    os.setf (fl);
    
    return os;
}
