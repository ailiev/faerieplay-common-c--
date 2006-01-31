/* -*- c -*-
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
 * comm_types.x
 * alex iliev, jan 2003
 * XDR types for the C types to be transported card<->host
 */


/* pull in the Record_x definition */
/* %#include "record_x.h" */



typedef unsigned int index_t;

/* FIXME: causes a warning of duplicate declaration with the
   normal C size_t. don't see how to remove that though.
*/
typedef unsigned int size_t;


typedef opaque ByteBuffer_x<>;

/* and a list of them ... */
typedef ByteBuffer_x list_ByteBuffer_x<>;


enum host_status_t {
    STATUS_OK = 0,
    STATUS_COMM_FAILURE,	/* communication to/from the host failed */
    STATUS_BAD_PARAM,		/* some invalid parameter */
    STATUS_IO_ERROR,
    STATUS_DIR_ERROR		/* some directory related error */
};



/* object_id is a (semantically hierarchical) list of indices
 * eg. for a record, index[0] is the bucker number, and index[1] is the record
 * inside that bucket
 */
 
const OID_MAXLEVELS  = 2;

struct object_id_x {
    index_t indices[OID_MAXLEVELS];
    size_t levels;
};


struct blob_x {
    ByteBuffer_x bytes;
};




/*
 * structs to be used for reading and writing of files
 *
 * HACK: the item name can be either an object_id or a string
 * should make a consistent and flexible naming scheme!
 *
 * well, this may not be so bad - whatever fits into the object_id scheme can
 * use that, all others use strings!
 */

enum object_name_scheme_t {
    NAME_STRING,
    NAME_OBJECT_ID
};


union object_name_x switch (object_name_scheme_t name_scheme) {
 case NAME_OBJECT_ID:
     object_id_x oid_name;
 case NAME_STRING:
     string    str_name<>;
};


struct blob_address_x {
    string basedir<>;
    object_name_x name;
};


typedef blob_address_x list_blob_address_x<>;


struct named_blob_x {
    blob_address_x address;
    ByteBuffer_x blob;
};


typedef named_blob_x list_named_blob_x<>;



/* type to specify container parameters for HOST_CREATE_CONTAINER
 */
struct container_params_x {
    string name<>;
    size_t num_objs;
    size_t max_obj_size;
};


/* an explicit type for a string, so we can use the XDR_STRUCT class on it */
typedef string string_x<>;



/* a pair of strings */
struct string_pair_x {
    string a<>;
    string b<>;
};

/* pair of size_t */
struct size_t_pair_x {
    size_t a;
    size_t b;
};


/*
  type for 'pirsearch' to send to the pirserver: a name to find or a
  record to update
*/
enum pir_request_type_x {
    PIRREQ_RETRIEVE,
    PIRREQ_UPDATE
};


/* Mon Jan 30 17:50:06 2006: disable for now, during SFDL devel */
/*
union pir_request_x switch (pir_request_type_x reqtype) {
 case PIRREQ_RETRIEVE:
     string recname<>;
 case PIRREQ_UPDATE:
     Record_x rec;
};
*/


/* arg for HOST_WRITE_HEADER */
struct write_header_arg_x {
    string  basedir<>;

    string name<>;

    ByteBuffer_x val;
};

struct read_header_arg_x {
    string  basedir<>;

    string name<>;
};



/*
 * types of notifications to be sent to the host
 */
enum host_notice_t {
    NOTICE_SHUFFLE_DONE
};



/*
 * types used by the actual calling and receiving functions. these types won't
 * be seen by the "application code"
 */
enum host_service_id_t {
    HOST_FETCH_BLOB,
    HOST_FETCH_CLEARRECORD,
    HOST_WRITE_BLOB,

    /*
      the new io services
    */

    /* takes an encoded blob_address_x, and returns an unencoded byte array (ie.
     * just the bytes.)
     */
    HOST_READ_BLOB_NEW,

    /* read a list of blobs
     * send host a list_blob_address_x
     * get back a list_ByteBuffer_x
     */
    HOST_READ_BLOBS,

    /* takes a named_blob_x, and returns nothing */
    HOST_WRITE_BLOB_NEW,

    /* write a list of blobs
     * send host a list_named_blob_x
     * nothing to return
     */
    HOST_WRITE_BLOBS,

    /* create a new container with a given name and sizes */
    HOST_CREATE_CONTAINER,

    
    /*
     * read or write to the header of a container
     */
    
    /* takes: cont name :: string<>
              value     :: ByteBuffer_x
	      offset	:: size_t
     * return: void
     */
    HOST_WRITE_HEADER,
    
    /* takes: cont name	    :: string<>
              offset, len   :: size_t
       return: ByteBuffer_x
    */
    HOST_READ_HEADER,

    

    /* takes:   cont name :: string<>
     * returns: size_t
     * returns the number of elements in the named container
     * */
    HOST_GET_CONT_LEN,

    /* takes:	string_pair_x : the old name and the new name
     * returns: nothing
     * renames the container, overwriting an existing one if it exists
     */
    HOST_RENAME_CONTAINER,

    /* takes: string_pair_x : the src container and the dest
     * returns: nothing
     * copies the src container to so it has name dest
     * an existing 'dest' container is overwritten
     */
    HOST_COPY_CONTAINER,

    
    HOST_NOTICE
};


struct host_call_buffer {
    host_service_id_t service_id;
    ByteBuffer_x params;
};

struct host_return_buffer {
    host_status_t status;
    ByteBuffer_x result;
};

