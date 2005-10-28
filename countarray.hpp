
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
#ifndef COUNTED_ARRAY_HPP
#define COUNTED_ARRAY_HPP

/* class for counted reference semantics
 * - deletes the object to which it refers when the last CountedPtr
 *   that refers to it is destroyed
 */

#include <string>

#ifndef NDEBUG
#include <iostream>
#endif

#include "comm_types.h"
#include "utils.h"

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
    
    enum should_free_t {
	do_free,
	no_free
    };

    // indicate that a constructor should perform a deep copy
    enum deepcopy_singleton_t {
	DEEPCOPY
    };


    enum copy_depth_t {
	DEEP,
	SHALLOW
    };
    

private:
    size_t _len;		// length of the array
    byte* ptr;			// pointer to the value

    bool is_owner;		// should we free the pointer at the end?
    ssize_t* count;		// shared number of owners
				// NOTE: 'count' is only used for owner
				// Arrays, ie. if !is_owner, count = NULL

public:

    // default init, blank state not good for anything
    CountedByteArray ()
	: _len		(0),
	  ptr		(NULL),
	  is_owner	(false),
	  count		(NULL)
	{}
    
    // initialize by allocating a buffer of a given size
    explicit CountedByteArray (size_t l) :
	_len		(l),
	ptr		(new byte[_len]),
	is_owner	(true),
	count		(new ssize_t(1))
	{}
    
    // initialize pointer with existing pointer and length
    explicit CountedByteArray (void * p, size_t l,
			       should_free_t should_free = do_free)
	: _len		(l),
	  ptr		(static_cast<byte*> (p)),
	  is_owner	(should_free == do_free ? true : false),
	  count		(is_owner ? new ssize_t(1) : NULL)
	{}


    // init from a C++ char string
    // clearly this should not be freed in this object!
    // what a pain in the butt with all the casting!
    CountedByteArray (const std::string& str,
		      copy_depth_t depth = SHALLOW)
	: _len		(str.length()),
	  ptr		(reinterpret_cast<byte*>
			 (const_cast<char*> (str.data()))),
	  is_owner	(false),
	  count		(NULL)
	{
	    if (depth == DEEP) {
		// redo a few things...
		ptr = new byte[_len];
		str.copy (cdata(), len());
		
		is_owner = true;
		count = new ssize_t(1);
	    }
	}
    
    
    // copy pointer (one more owner)
    CountedByteArray (const CountedByteArray& p) throw()
     : _len	(p._len),
       ptr	(p.ptr),
       is_owner	(p.is_owner),
       count	(p.count)
	{
	    if (is_owner) ++*count;
	}

    // deep copy
    CountedByteArray (const CountedByteArray& b, deepcopy_singleton_t d) :
	_len	(b._len),
	ptr	(new byte[_len]),
	is_owner(true),
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
	: _len		(size),
	  ptr		(b.ptr + start),
	  is_owner	(false),
	  count		(NULL)
	{}


    // init from an XDR representation - deep copy
    CountedByteArray (const ByteBuffer_x & buf)
	:  _len        (buf.ByteBuffer_x_len),
	   ptr         (new byte[_len]),
	   is_owner	(true),
	   count       (new ssize_t(1))
	{
	    memcpy (ptr, buf.ByteBuffer_x_val, _len);
	}

    // WARNING: careful with the lifetime of the returned structure! This is
    // just a shallow copy
    ByteBuffer_x to_xdr () const {
	// HACK: cant do the const stuff any better here :(
	ByteBuffer_x answer = { len(),
				const_cast<char*> (cdata()) };
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

            ptr =	p.ptr;
	    _len =	p._len;
            count =	p.count;
	    is_owner =	p.is_owner;

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
	    std::cerr
		<< "WARNING: Write access of data on ByteBuffer with aliases!"
		<< std::endl;
	}
#endif
	return ptr;
    }

    char* cdata () throw() {
#ifndef NDEBUG
	if (is_owner && *count > 1) {
	    std::cerr << "Write access of cdata on ByteBuffer with aliases!"
		      << std::endl;
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
	
};

#endif /*COUNTED_ARRAY_HPP*/
