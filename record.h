/*
 * ** PIR Private Directory Service prototype
 * ** Copyright (C) 2002 Alexander Iliev <iliev@nimbus.dartmouth.edu>
 * **
 * ** This program is free software; you can redistribute it and/or modify
 * ** it under the terms of the GNU General Public License as published by
 * ** the Free Software Foundation; either version 2 of the License, or
 * ** (at your option) any later version.
 * **
 * ** This program is distributed in the hope that it will be useful,
 * ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 * ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * ** GNU General Public License for more details.
 * **
 * ** You should have received a copy of the GNU General Public License
 * ** along with this program; if not, write to the Free Software
 * ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * */

// -*-c++-*-
// record.h
// alex iliev, nov 2002
// the C++ class to represent DB records
//


#ifndef __RECORD_H_
#define __RECORD_H_

#include <string>
#include <list>

#include <common/utils.h>

#include "record_x.h"


struct RecordAttr {

    RecordAttr () {}
    
    RecordAttr (std::string name, std::string value) :
	name(name), value(value) {}
    
    std::string name, value;
};


class Record {

public:
    
    typedef std::list<RecordAttr> attrlist_t;
    
    Record () {}

    // construct via de-serialize, via XDR
    Record (const ByteBuffer& in);


//    Record& operator= (const Record& src);
    
    // copy into an XDR record
    void to_xdr (Record_x & out) const;

    // serialize, via XDR
    ByteBuffer serialize () const;

    // reconstruct, via XDR
    void reconstruct (const ByteBuffer& in);
    


    std::string name;
    attrlist_t attributes;

};


// just to show off
std::ostream& operator<< (std::ostream&, const Record&);


#endif // __RECORD_H_
