
/* The following code example is taken from the book
 * "The C++ Standard Library - A Tutorial and Reference"
 * by Nicolai M. Josuttis, Addison-Wesley, 1999
 *
 * (C) Copyright Nicolai M. Josuttis 1999.
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 */

/* class for counted reference semantics
 * - deletes the object to which it refers when the last CountedPtr
 *   that refers to it is destroyed
 */

#include <string>
#include <string.h>		// memset

#include <boost/none.hpp>

#ifndef NDEBUG
#include <iostream>
#endif

#include <common/logging.h>
#include <common/utils-types.h>

#include "comm_types.h"



#ifndef COUNTED_ARRAY_HPP
#define COUNTED_ARRAY_HPP



// modified by alex iliev, nov 2002
// to use for array objects (ie use delete[] and carry a length), and rename
// appropriately
// to give option to not free at the end (eg if array is not dynamically
// allocated)

// march 30, 2003:
// removed the template and made it only for bytes

// jan 2004:
// improved the const vs. non-const data() methods




class CountedByteArray {

public:
    
//     enum should_free_t {
// 	do_free,
// 	no_free
//     };

    static const size_t MAXLEN = 1<<20;
    
    // indicate that a constructor should perform a deep copy
    struct deepcopy {};


    enum copy_depth_t {
	DEEP,			/// deep copy, take ownership
	SHALLOW,		/// shallow copy, no ownership
	OWNER			/// shallow copy, take ownership, DEFAULT
    };
    

private:
    bool is_owner;		// should we free the pointer at the end?
    size_t _len;		// length of the array
    byte* ptr;			// pointer to the value

    ssize_t* count;		// shared number of owners
				// NOTE: 'count' is only used for owner
				// Arrays, ie. if !is_owner, count =
				// NULL


    static Log::logger_t logger;

public:

    /// default init, blank state not good for anything, just to be replaced.
    CountedByteArray ()
	: is_owner	(false),
	  _len		(0),
	  ptr		(NULL),
	  count		(NULL)
	{}
    
    /// initialize by allocating a buffer of a given size
    explicit CountedByteArray (size_t l) :
	is_owner	(true),
	_len		(l),
	ptr		(new byte[_len]),
	count		(new ssize_t(1))
	{}

    // three possibilities:
    // - want to take ownership of the pointer,
    // - want to not take ownership and:
    //   - shallow copy
    //   - deep copy (includes taking ownership)
    
    /// initialize with a given pointer
    /// @param depth #copy_depth_t specifying how to absorb the pointer.
    explicit CountedByteArray (void * p, size_t l,
			       copy_depth_t depth = OWNER)
	: is_owner	(depth == OWNER || depth == DEEP),
	  _len		(l),
	  ptr		(depth == DEEP ? new byte[_len] : static_cast<byte*> (p)),
	  count		(is_owner ? new ssize_t(1) : NULL)
	{
	    if (depth == DEEP)
	    {
		memcpy (ptr, p, _len);
	    }
	}

    // separate instance taking a const void*
    explicit CountedByteArray (const void * p, size_t l,
			       deepcopy)
	: is_owner (true),
	  _len (l),
	  ptr (new byte[_len]),
	  count (new ssize_t(1))
	{
	    memcpy (ptr, p, _len);
	}


    /// init from a C++ char string.
    // what a pain in the butt with all the casting!
    CountedByteArray (const std::string& str,
		      copy_depth_t depth = SHALLOW)
	: is_owner	(depth == DEEP),
	  _len		(str.length()),
	  ptr		(depth == DEEP			    ?
			 new byte[_len]			    :
			 reinterpret_cast<byte*> (
			     const_cast<char*> (str.data()))),
	  count		(is_owner ? new ssize_t(1) : NULL)
	{
	    if (depth == DEEP)
	    {
		str.copy (cdata(), _len);
	    }
	}
    
    
    /// copy pointer (one more owner)
    CountedByteArray (const CountedByteArray& p) throw()
     : is_owner	(p.is_owner),
       _len	(p._len),
       ptr	(p.ptr),
       count	(p.count)
	{
	    if (is_owner) ++*count;
	}

    /// deep copy constructor
    CountedByteArray (const CountedByteArray& b, deepcopy) :
	is_owner(true),
	_len	(b._len),
	ptr	(new byte[_len]),
	count	(new ssize_t(1))
	{
	    memcpy (ptr, b.ptr, _len);
	}

    // make an alias into an existing array
    // FIXME: they should really use the same count variable, but then i need to
    // have another member to point to the start of the original memory.
    // WARN: this does not tell the owner ByteArray that there is a new
    // reference, so if the owner goes out of scope, so will this alias!
    // TODO: maybe write another class which contains this extra member, like
    // ByteAlias, which then is a full-fledged reference to the owner
    CountedByteArray (const CountedByteArray& b, size_t start, size_t size)
	: is_owner	(false),
	  _len		(size),
	  ptr		(b.ptr + start),
	  count		(NULL)
	{
	    assert (start + size <= b.len());
	}


    // init from an XDR representation - deep copy
//     CountedByteArray (const ByteBuffer_x & buf)
// 	:  is_owner	(true),
// 	   _len        (buf.ByteBuffer_x_len),
// 	   ptr         (new byte[_len]),
// 	   count       (new ssize_t(1))
// 	{
// 	    memcpy (ptr, buf.ByteBuffer_x_val, _len);
// 	}

    /// init from an XDR representation
    CountedByteArray (const ByteBuffer_x & buf,
		      copy_depth_t deep = DEEP)
	:  is_owner	(deep == DEEP),
	   _len		(buf.ByteBuffer_x_len),
	   ptr		(deep == DEEP	    ?
			 new byte[_len]	    :
			 reinterpret_cast<byte*> (buf.ByteBuffer_x_val)),
	   count	(is_owner ? new ssize_t(1) : NULL)
	{
	    if (deep == DEEP)
	    {
		memcpy (ptr, buf.ByteBuffer_x_val, _len);
	    }
	}

    bool operator== (const CountedByteArray& b)
	{
	    return _len == b._len &&
		memcmp (ptr, b.ptr, _len) == 0;
	}

    bool operator!= (const CountedByteArray& b)
	{
	    return ! operator== (b);
	}
    



    
    void set (byte b)
	{
	    memset (ptr, b, _len);
	}

    // WARNING: careful with the lifetime of the returned structure! This is
    // just a shallow copy
    ByteBuffer_x to_xdr (copy_depth_t deep = SHALLOW) const {
	assert (_len < MAXLEN);

	ByteBuffer_x answer = { _len,
				deep == DEEP ?
				new char[_len] :
				const_cast<char*> (cdata()) };
	if (deep == DEEP)
	{
	    memcpy (answer.ByteBuffer_x_val, ptr, _len);
	}
	
	return answer;
    }
    
    // destructor (delete value if this was the last owner)
    ~CountedByteArray () throw() {
        dispose();
    }

    // assignment (unshare old and share new value)
    CountedByteArray& operator= (const CountedByteArray& p) throw() {
        if (this != &p) {
            dispose();

	    ptr		= p.ptr;
	    _len	= p._len;
            count	= p.count;
	    is_owner	= p.is_owner;

            if (is_owner) ++*count;
        }
        return *this;
    }

    
    // access or modify the length
    size_t& len () throw() {
	return _len;
    }

    // access the data, but issue warning if object has aliases
    byte* data () throw() {
#ifndef NDEBUG
	if (is_owner && *count > 1) {
	    LOG (Log::WARN, logger,
		 "WARNING: Write access of data on ByteBuffer with aliases!")
	}
#endif
	return ptr;
    }

    char* cdata () throw() {
#ifndef NDEBUG
	if (is_owner && *count > 1) {
	    LOG (Log::WARN, logger,
		 "WARNING: Write access of cdata on ByteBuffer with aliases!")
	}
#endif

	return reinterpret_cast<char*> (ptr);
    }


    //
    // and const versions
    //
    size_t len() const {
	return _len;
    }

    const byte* data () const throw() {
	return ptr;
    }

    // wrap up the nasty cast to signed char pointer
    const char* cdata() const throw() {
	return reinterpret_cast<char*> (ptr);
    }

    
    

private:
    void dispose() {
	if (is_owner && --*count == 0) {
	    delete    count;
	    delete [] ptr;
	}
    }

public:

    DECL_STATIC_INIT (logger = Log::makeLogger ("bytebuffer"));
};


DECL_STATIC_INIT_INSTANCE(CountedByteArray);    



// some helpers

/// make a basic C type from a ByteBuffer, by copying out the bytes
template <class T>
T
bb2basic (const CountedByteArray& buf)
{
    T t;
    assert (sizeof(t) == buf.len());

    memcpy (&t, buf.data(), buf.len());

    return t;
}

/// make a ByteBuffer out of a basic type
template <class T>
CountedByteArray
basic2bb (const T& t)
{
    return CountedByteArray (&t, sizeof(t), CountedByteArray::deepcopy());
}

inline
CountedByteArray
string2bb (const std::string& s)
{
    return CountedByteArray (s, CountedByteArray::DEEP);
}

#ifndef NDEBUG
std::ostream& operator<< (std::ostream&, const CountedByteArray&);
#endif


typedef CountedByteArray ByteBuffer;


#endif /*COUNTED_ARRAY_HPP*/
