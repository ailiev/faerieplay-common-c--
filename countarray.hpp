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
#ifndef COUNTED_PTR_HPP
#define COUNTED_PTR_HPP

/* class for counted reference semantics
 * - deletes the object to which it refers when the last CountedPtr
 *   that refers to it is destroyed
 */

// modified by alex iliev, nov 2002
// to use for array objects (ie use delete[] and carry a length), and rename
// appropriately
// to give option to not free at the end (eg if array is not dynamically
// allocated)
template <class T>
class CountedArray {

public:
    
    enum should_free_t {
	do_free,
	no_free
    };
    

private:
    T* ptr;			// pointer to the value
    size_t _len;			// length of the array
    int* count;			// shared number of owners
    should_free_t should_free;	// should we free the pointer at the end?

public:

    // initialize pointer with existing pointer and length
    explicit CountedArray (T* p=0, size_t l = 0,
			   should_free_t should_free = do_free)
	: ptr(p), _len(l), count(new int(1)), should_free(should_free) {}

    // copy pointer (one more owner)
    CountedArray (const CountedArray<T>& p) throw()
     : ptr(p.ptr), _len(p._len), count(p.count), should_free(p.should_free) {
        ++*count;
    }

    // destructor (delete value if this was the last owner)
    ~CountedArray () throw() {
        dispose();
    }

    // assignment (unshare old and share new value)
    CountedArray<T>& operator= (const CountedArray<T>& p) throw() {
        if (this != &p) {
            dispose();
            ptr = p.ptr;
	    _len = p._len;
            count = p.count;
	    should_free = p.should_free;
            ++*count;
        }
        return *this;
    }

    
    // access or modify the length
    size_t& len () throw() {
	return _len;
    }


    //
    // and const versions
    //
    size_t len() const {
	return _len;
    }

    T* data () const throw() {
	return ptr;
    }
    

#if 0
    // access the value to which the pointer refers
    T& operator*() const throw() {
        return *ptr;
    }
    T* operator->() const throw() {
        return ptr;
    }
#endif // 0

  private:
    void dispose() {
        if (--*count == 0) {
             delete count;
             if (should_free == do_free) delete [] ptr;
        }
    }
    
};

#endif /*COUNTED_PTR_HPP*/
