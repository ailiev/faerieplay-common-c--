/* -*-c-*-
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
 * record_x.x: The XDR record type, consisting of a name and a list of
 *                name-value attributes
 * alex iliev, jan 2003
 */ 


struct attr_x
{
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


typedef Record_x list_Record_x<>;
