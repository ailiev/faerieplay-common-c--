/*
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

/* -*- c++ -*-
 * exceptions.h
 * alex iliev, nov 2002
 * exception classes for the private directory code
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
