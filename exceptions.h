/* -*- c++ -*-
 *
 */

#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H


#include <string>
#include <vector>
#include <exception>

#include <iostream>
#include <sstream>

#include <common/hostcall.h>


// our own better interface for an exception

class better_exception : public std::exception {

public:
    
    better_exception (const std::string& msg = "Better Exception")
	: std::exception (), _msg (msg) {}

    better_exception (const better_exception& cause,
		      const std::string& msg);

    
    // from the exception class
    virtual const char* what() const throw() {
	return _msg.c_str();
    }
    
    operator std::string () {
	return _msg;
    }
    
    virtual ~better_exception() throw() {}
    
private:

//    std::vector<better_exception> _cause_stack;
    std::string _msg;
};





class crypto_exception : public better_exception {

public:
    
    crypto_exception (const std::string& msg) :
	better_exception (msg) {}

    
    crypto_exception (const better_exception& cause,
		      const std::string& msg = "")
	: better_exception (cause, msg) {}
    
    
    virtual ~crypto_exception() throw() {}
};




//
// an exception to indicate some host failure
//
class host_exception : public better_exception {
    
public:

    host_exception (const std::string & msg, host_status_t status) :
	better_exception (make_msg (msg, status)),
	status(status)
	{}

    virtual ~host_exception () throw () {}

    host_status_t status;

private:

    std::string make_msg (const std::string & msg, host_status_t status) {
	std::ostringstream os;
	os << msg << "; status = " << status;
	return os.str();
    }

};



#endif // _EXCEPTIONS_H
