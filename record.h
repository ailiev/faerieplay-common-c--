// -*-c++-*-
// record.h
// the C++ class to represent DB records
//


#ifndef __RECORD_H_
#define __RECORD_H_

#include <string>
#include <list>

#include <common/utils.h>

#include "record_x.h"


struct RecordAttr {
    std::string name, value;
};


class Record {

public:
    
    typedef std::list<RecordAttr> attrlist_t;
    
    Record () {}

    // initialize from an XDR record
    Record (const Record_x& xdr_rec);

    // de-serialize, via XDR
    Record (const ByteBuffer& in);


    // copy into an XDR record
    void to_xdr (Record_x & out) const;

    // serialize, via XDR
    ByteBuffer serialize () const;


    std::string name;
    attrlist_t attributes;

};


// just to show off
std::ostream& operator<< (std::ostream&, const Record&);


#endif // __RECORD_H_
