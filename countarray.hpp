
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
    

private:
    size_t _len;		// length of the array
    byte* ptr;			// pointer to the value

    ssize_t* count;		// shared number of owners
    bool is_owner;		// should we free the pointer at the end?

public:

    // initialize pointer with existing pointer and length
    explicit CountedByteArray (byte * p = (byte*)0, size_t l = 0,
			       should_free_t should_free = do_free)
	: _len(l), ptr(p), count(new ssize_t(1)),
	  is_owner (should_free == do_free ? true : false)
	{}


    // init from a C++ char string
    // clearly this should not be freed in this object!
    // what a pain in the butt with all the casting!
    CountedByteArray (const std::string& str)
	: _len        (str.length()),
	  ptr         (reinterpret_cast<byte*>
		       (const_cast<char*> (str.data()))),
	  count       (new ssize_t(1)),
	  is_owner (false) {}
    
    
    // copy pointer (one more owner)
    CountedByteArray (const CountedByteArray& p) throw()
     : _len(p._len), ptr(p.ptr), count(p.count), is_owner(p.is_owner)
	{
	    ++*count;
	}

    // make an alias into an existing array
    // FIXME: they should really use the same count variable, but then i need to
    // have another member to point to the start of the original memory
    // TODO: maybe write another class which contains this extra member, like
    // ByteAlias
    CountedByteArray (const CountedByteArray& b, size_t start, size_t size)
	: _len        (size),
	  ptr         (b.ptr + start),
	  count       (new ssize_t(1)),
	  is_owner (false)
	{}


    CountedByteArray (const ByteBuffer_x & buf)
	:  _len        (buf.ByteBuffer_x_len),
	   ptr         (new byte[_len]),
	   count       (new ssize_t(1)),
	   is_owner (true)
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
            ptr = p.ptr;
	    _len = p._len;
            count = p.count;
	    is_owner = p.is_owner;
            ++*count;
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
	if (*count > 1) {
	    std::cerr << "Write access of data on ByteBuffer with aliases!"
		      << std::endl;
	}
#endif
	return ptr;
    }

    char* cdata () throw() {
#ifndef NDEBUG
	if (*count > 1) {
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
    

#if 0
    // access the value to which the pointer refers
    byte& operator*() const throw() {
        return *ptr;
    }
    byte* operator->() const throw() {
        return ptr;
    }
#endif // 0

  private:
    void dispose() {
        if (--*count == 0) {
             delete count;
             if (is_owner) delete [] ptr;
        }
    }
    
};

#endif /*COUNTED_ARRAY_HPP*/
