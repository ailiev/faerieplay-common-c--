//
// the Record class
//

#include <string>

#include <iostream>

#include <common/xdr_class.h>

#include "record.h"
#include "record_x.h"


using namespace std;


Record::Record (const Record_x& src) {

    name = src.name;

    attrlist_x *current = src.attributes;

    while (current) {
	RecordAttr attr;
	attr.name = current->attr.name;
	attr.value = current->attr.value;

	attributes.push_back (attr);

	current = current->next;
    }

}



Record::Record (const ByteBuffer& in) {

    XDRStruct<Record_x, xdr_Record_x> xdr;
    xdr.decode (in);
    Record (xdr.x);
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

    Record_x x;
    to_xdr (x);
    
    XDRStruct<Record_x, xdr_Record_x> xdr (x);
    return xdr.encode();
}




ostream& operator<< (ostream& os, const Record& r) {

    os << "----" << endl;
    os << "Record " << r.name << endl;

    for (Record::attrlist_t::const_iterator i = r.attributes.begin();
	 i != r.attributes.end();
	 i++)
    {
	os << i->name << " = " << i->value << endl;
    }

    os << "----" << endl;
    return os;
}
