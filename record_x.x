/* -*-c-*-
 * record_x.x: The XDR record type, consisting of a name and a list of
 *                name-value attributes
 * alex iliev, jan 2003
 */ 


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
