/* -*- c++ -*- */
/*
 * Library routines for the Dartmouth CS Private Information Retrieval and
 * Writing (PIR/W) library.
 *
 * Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu> and
 * Sean W. Smith <sws@cs.dartmouth.edu>
 *
 * All rights reserved.
 *
 * This code is released under a BSD license.
 * Please see LICENSE.txt for the full license and disclaimers.
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com)
*/


/* 
 * exceptions.h
 * alex iliev, nov 2002
 * exception classes for the private directory code
 */


#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H



#include <string>
#include <exception>
#include <memory>

#include <iostream>
#include <sstream>

#include <execinfo.h>		// for backtrace and backtrace_symbols

#include <typeinfo>

#include <stdlib.h>
#include <string.h>

///
/// Send an exception to a stream.
std::ostream & operator<< (std::ostream & os, const std::exception& ex);

    
///
/// our own better interface for an exception.
class better_exception : public std::exception {

public:
    
    better_exception (const std::string& msg = "Better Exception")
	: std::exception (),
	  _cause         (),
	  _msg           (msg)
	{
	    //
	    // produce a backtrace
	    //
	    void * array[25];
	    int nSize = backtrace(array, 25);
	    char ** symbols = backtrace_symbols(array, nSize);
	    
	    for (int i = 0; i < nSize; i++)
	    {
		std::cerr << symbols[i] << std::endl;
	    }
	    
	    free(symbols);

// trigger a SEGV, so we can get a core dump	    
//	    * ((int*) (0x0)) = 42;
	}

    ///
    /// copy.
    /// NOTE: not same as initializing with a cause.
    better_exception (const better_exception& src)
	: std::exception (),
	  _cause         (src._cause),
	  _msg           (src._msg)
	{}

    ///
    /// initialize with a cause
    better_exception (const better_exception& cause,
		      const std::string& msg)
	: std::exception (),
	  _cause         (&cause),
	  _msg           (msg)
	{
	    _msg += " (caused by " + cause._msg + ")";
	}

    virtual std::string make_msg (const std::string& msg) const
	{
	    return (std::string) typeid(*this).name() + ": " + msg;
	}

    
    /// override from the exception class
    virtual const char* what() const throw() {
	return _msg.c_str();
	// TODO: could this be a bad time to be allocating strings etc?
//	_stored_msg = make_msg (_msg);
//	return _stored_msg.c_str();
    }
    
    /// Convert to string.
    operator std::string () {
	return _msg;
    }
    
    virtual ~better_exception() throw() {}

    
private:

    const better_exception * _cause;
    std::string _msg;

//    mutable std::string _stored_msg;
};



class runtime_exception : public better_exception {

public:

    runtime_exception (const std::string& msg = "Runtime Exception")
	: better_exception (msg) {}

};



class bad_arg_exception : public runtime_exception {

public:
    
    bad_arg_exception (const std::string& msg = "Bad Argument Exception")
	: runtime_exception (msg) {}

};


class unsupported_operation_exception : public runtime_exception
{

public:
    
    unsupported_operation_exception
    (const std::string& msg = "Unsupported Operation Exception")
	: runtime_exception (msg) {}

};



class invalid_state_exception : public runtime_exception {

public:

    invalid_state_exception (const std::string& msg = "Invalid State Exception")
	: runtime_exception (msg) {}
};



class parse_exception : public better_exception
{

public:
    
    parse_exception (const std::string& msg = "Parse exception") :
	better_exception (msg) {}

    
    parse_exception (const better_exception& cause,
		      const std::string& msg = "Parse exception")
	: better_exception (cause, msg) {}
    
};


class crypto_exception : public better_exception {

public:
    
    crypto_exception (const std::string& msg) :
	better_exception (msg) {}

    
    crypto_exception (const better_exception& cause,
		      const std::string& msg = "")
	: better_exception (cause, msg) {}
};



class mac_failure_exception : public crypto_exception {

public:

    mac_failure_exception (const std::string& msg = "MAC check failed") :
	crypto_exception (msg) {}

    mac_failure_exception (const better_exception& cause,
			   const std::string& msg = "")
	: crypto_exception (cause, msg) {}
    
    
    virtual ~mac_failure_exception() throw() {}
};


class hash_failure_exception : public crypto_exception {

public:

    hash_failure_exception (const std::string& msg = "Hash check failed") :
	crypto_exception (msg) {}

    hash_failure_exception (const better_exception& cause,
			    const std::string& msg = "")
	: crypto_exception (cause, msg) {}
    
    
    virtual ~hash_failure_exception() throw() {}
};





class xdr_exception : public better_exception {

public:

    xdr_exception (const std::string& msg) :
	better_exception ("XDR error: " + msg) {}

    virtual ~xdr_exception ()  throw() {}
};




//
// exception to indicate communication failure
//
class comm_exception : public better_exception {

public:

    comm_exception (const std::string & msg) : better_exception (msg) {}

};




//
//
// exceptions for host use...
//
//

class io_exception : public better_exception {

public:

    io_exception (const std::string & msg)
	: better_exception (msg)
	{}

    io_exception (const std::string & msg,
		  int errno_now)
	: better_exception (mkmsg (msg, errno_now))
	{}

    io_exception (const io_exception& cause,
		  const std::string & msg="")
	: better_exception (cause, msg)
	{}

    

private:

    static std::string mkmsg (const std::string& m1, int err) {
	return m1 + " : " + strerror(err);
    }
};


#endif // _EXCEPTIONS_H
