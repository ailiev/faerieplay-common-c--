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


/*
record_id -> problem with having constructors, becomes inconvenient
             no it's not! use struct initialization
   index_t -> define in XDR? why not

encrypted_record
   ByteBuffer -> wrap as a byte array in XDR

named_record
   ByteBuffer
   record_id

host_status_t : enum, define in XDR


*/

typedef unsigned int index_t;

typedef opaque ByteBuffer_x<>;


enum host_status_t {
    STATUS_OK = 0,
    STATUS_COMM_FAILURE,	/* communication to/from the host failed */
    STATUS_BAD_PARAM,		/* some invalid parameter */
    STATUS_IO_ERROR,
    STATUS_DIR_ERROR		/* some directory related error */
};


struct object_id
{
    index_t bucket;
    index_t record;
};


struct record_id
{
    index_t bucket;
    index_t record;
};


struct named_blob_x {
    object_id id;
    ByteBuffer_x rec_bytes;
};


struct blob_x {
    ByteBuffer_x bytes;
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

