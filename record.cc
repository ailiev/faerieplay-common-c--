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

/*
 * record.cc
 * alex iliev, nov 2002
 * the Record class
 */


#include <string>

#include <iostream>
#include <fstream>

#include <common/xdr_class.h>

#include "record.h"
#include "record_x.h"


using namespace std;


static void
free_record (Record_x & rec);



#ifdef _TESTING_RECORD

int main () {

    Record r;
    r.name = "kuku";

    r.attributes.push_back (RecordAttr ("age", "old"));
    r.attributes.push_back (RecordAttr ("color", "navy blue"));

    cout << r << endl;


    ByteBuffer b = r.serialize();

    Record r2 (b);

    cout << r2 << endl;
}

#endif // _TESTING_RECORD



Record::Record (const ByteBuffer& in) {
    reconstruct (in);
}



// reconstruct, via XDR
void Record::reconstruct (const ByteBuffer& in) {

    XDR_STRUCT(Record_x) xdr;
    xdr.decode (in);

    name = xdr.x.name;

    attrlist_x *current = xdr.x.attributes;
    attributes.clear();
    RecordAttr attr;
    while (current) {
	attr.name = current->attr.name;
	attr.value = current->attr.value;

	attributes.push_back (attr);

	current = current->next;
    }
}


void Record::to_xdr (Record_x & out) const {

    attrlist_x *last=NULL, *current=NULL;

    out.name = strdup (name.c_str());
    
    // and now the attributes
    out.attributes = (attrlist_x*) malloc (sizeof (attrlist_x));
    current = out.attributes;

    for (attrlist_t::const_iterator i = attributes.begin();
	 i != attributes.end();
	 i++)
    {

	current->attr.name = strdup (i->name.c_str());
	current->attr.value = strdup (i->value.c_str());

	// allocate next node
	current->next = (attrlist_x*) malloc (sizeof (attrlist_x));

	last = current;		// advance pointer
	current = current->next;
    }


    // free the last node, it's not needed
    free (current);
	
    // reset the last pointer to indicate end of list
    if (last == NULL) {
	// not even one attribute :(
	out.attributes = NULL;
    }
    else {
	last->next = NULL;
    }
}



ByteBuffer Record::serialize () const {

    Record_x rx;
    to_xdr (rx);
    
    XDR_STRUCT(Record_x) xdr (rx);
    ByteBuffer answer = xdr.encode();

    free_record (rx);

    return answer;
}




ostream& operator<< (ostream& os, const Record& r) {

    os << r.name << endl;

    for (Record::attrlist_t::const_iterator i = r.attributes.begin();
	 i != r.attributes.end();
	 i++)
    {
	os << i->name << endl << i->value << endl;
    }

    return os;
}


//
// to free a Record_x created in to_xdr
//
static void
free_record (Record_x & rec) {

    free (rec.name);

    if ( ! rec.attributes) {
	return;
    }

    attrlist_x *current = rec.attributes, *next;
    while (current) {
	free (current->attr.name);
	free (current->attr.value);
	
	next = current->next;
	free (current);
	current = next;
    }

    return;
}

