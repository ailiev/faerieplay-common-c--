/*
 * record_x.x: Remote directory listing protocol
 */ 

/* const MAXSTRING = 256; */


struct attr_x {
    string name<>;
    string value<>;
};

struct attrlist_x
{
    attr_x attr;
    attrlist_x * next;
};


struct Record_x
{
    string name<>;
    attrlist_x * attributes;
};
