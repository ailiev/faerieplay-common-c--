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
 * utils.cc
 * alex iliev, nov 2002
 */

#include <istream>
#include <fstream>
#include <string>
#include <vector>

#include <stdio.h>
#include <math.h>

#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#include "utils.h"
#include "logging.h"

using std::string;
using std::vector;



namespace
{
    Log::logger_t logger = Log::makeLogger ("pir.common.utils");

    const string DIRSEP = "/";
}


    
// build the dir structure which contains 'name' - not the actual top object
// though
void builddirs (const string& name, mode_t mode)
    throw (io_exception)
{

    string dirname = name.substr (0, name.rfind (DIRSEP));

    
    int status = mkdir (dirname.c_str(), mode);
    if (status == 0 || errno == EEXIST) {
	if (status == 0) {
	    LOG (Log::DEBUG, logger, "Created dir " << dirname);
	}
	return;			// done
	// this may not be good if the dirname is actually an object
	// other than a directory, in which case we also get EEXIST
	// TODO: we should do a stat() and check
    }
    else if (errno != ENOENT) {
	goto shameful_egress;	// unrecoverable failure
    }
	
    // didnt find the parent dir - recurse
    builddirs (dirname, mode);

    // and redo the mkdir here
    status = mkdir (dirname.c_str(), mode);
    if (status != 0) {
	goto shameful_egress;
    }
    
    LOG (Log::DEBUG, logger, "Created dir " << dirname);

    return;			// success!

    
 shameful_egress:
    throw io_exception ("builddirs of " + name + " failed", errno);
}




void readfile (FILE * fh, string& into) throw (io_exception) {

    char buf[512];
    size_t read;

    into.clear();

    do {
	read = fread (buf, 1, sizeof(buf), fh);
	into.append (buf, read);
    } while (read == sizeof(buf));

    if (read == 0 && ferror(fh)) {
	// HACK: may not be correct to close fh here
	fclose (fh);
	throw io_exception ("Reading file", errno);
    }
}


void readfile (std::istream& is, std::string& into) throw (io_exception)
{
    char buf[512];
    int read;

    into.clear();
    // BUG: for some reason this readsome() does not work with cin, just returns
    // 0
    while ( ( read = is.readsome (buf, sizeof(buf)) ) > 0 && is.good() ) {
	into.append (buf, read);
    }

    if (is.bad()) {
	// serious failure, worse than eof
	throw io_exception ("Reading file", errno);
    }
}


// and a bit higher level:
ByteBuffer readfile (const std::string& name) throw (io_exception)
{
    std::ifstream ifs (name.c_str());
    if (ifs.fail()) {
	throw io_exception ("readfile: failed to open for reading " + name,
			    errno);
    }

    string buf;
    readfile (ifs, buf);

    return ByteBuffer (buf, ByteBuffer::DEEP);
}

void writefile (const std::string& name,
		const ByteBuffer& data)
    throw (io_exception)
{
    std::ofstream ofs (name.c_str());
    ofs.write (data.cdata(), data.len());
    if (ofs.bad()) {
	throw (io_exception ("writefile failed"));
    }
}


// FIXME: these are not the realloc(3) semantics, realloc could be done in place.
ByteBuffer
realloc_buf (const ByteBuffer& old, size_t new_size) {
    return ByteBuffer
	( static_cast<byte*>
	  (memcpy (new byte[new_size], old.data(),
		   std::min (old.len(),new_size))),
	  new_size );
}

string utoa (unsigned u) {

    if (u == 0) return "0";

    string answer;
    while (u > 0) {
	answer.insert (0, 1, '0' + char(u % 10));
	u /= 10;
    }
    return answer;
}




// work out a floor of lg(N), by repeated right shifting by 1
unsigned lgN_floor (int N) {

    int i;
    for (i = 0; N > 1; N >>= 1, i++)
	;

    return i;
}


unsigned lgN_ceil (int N) {

    int i;
    bool lsb_is_0 = true;	// are all the lsb's 0 as we shift right?
    for (i = 0; N > 1; N >>= 1, i++) {
	lsb_is_0 = lsb_is_0 && ((N & 0x1) == 0);
    }

    // at this point i has counted the number of bits in N's binary
    // form

    if (!lsb_is_0) {
	// this was not a power of two, so need to round up
	i++;
    }
    
    return i;
}



std::ostream & errmsg (std::ostream & os)
{
    return os << strerror(errno);
}



unsigned round_up (unsigned x, unsigned boundary) {
    unsigned rem = x % boundary;
    
    if (rem == 0) {
	return x;
    }
    else {
	return x + (boundary - rem);
    }
}


std::string itoa (int  n) {
    // 10 digits, a minus, and trailing null
    char s[12];
    snprintf(s, ARRLEN(s), "%d", n);
    return s;
} 


/*
ByteBuffer concat_bufs (const vector<ByteBuffer>& bufs)
{
    ByteBuffer answer;

    int sum = 0;
    FOREACH (b, bufs) {
	sum += b->len();
    }

    // allocate the new one
    answer = ByteBuffer (sum);

    int off = 0;
    FOREACH (b, bufs) {
	memcpy (answer.data() + off, b->data(), b->len());
	off += b->len();
    }
    
    return answer;
}
*/


std::ostream & epoch_time (std::ostream & os) {
    
    struct timeval tv;
    if ( gettimeofday (&tv, NULL) != 0 ) {
	os << "gettimeofday error " << strerror(errno);
	return os;
    }

    os << tv.tv_sec;
    return os;
}


time_t epoch_secs ()
{
    struct timeval tv;
    if ( gettimeofday (&tv, NULL) != 0 ) {
	return -1;
    }

    return tv.tv_sec;
}    


std::vector<std::string> split (const std::string& sep, const std::string& s)
{
    string::size_type start = 0, end = 0;
    vector<string> answer;
    while ( (end = s.find (sep, start)) != string::npos ) {
	if (end != start) {
	    answer.push_back (s.substr (start, end-start));
	}
	start = end+1;
    }

    // the final piece, without a separator at the end
    if (start < s.size()) {
	answer.push_back (s.substr (start, string::npos));
    }

    return answer;
}
