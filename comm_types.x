/* -*-c-*-
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
    STATUS_IO_ERROR,
    STATUS_DIR_ERROR
};


struct record_id
{
    index_t bucket;
    index_t record;
};


struct named_record_x {
    record_id id;
    ByteBuffer_x rec_bytes;
};


struct encrypted_record_x {
    ByteBuffer_x bytes;
};




/*
 * types used by the actual calling and receiving functions. these types won't
 * be seen by the "application code"
 */
enum host_service_id_t {
    HOST_FETCH_RECORD,
    HOST_FETCH_CLEARRECORD,
    HOST_WRITE_RECORD
};


struct host_call_buffer {
    host_service_id_t service_id;
    ByteBuffer_x params;
};

struct host_return_buffer {
    host_status_t status;
    ByteBuffer_x result;
};

