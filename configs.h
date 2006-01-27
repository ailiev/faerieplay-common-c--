// -*- c++ -*-
// structure for command line arguments

#include <string>

struct configs {

    std::string crypto_dir;
    std::string store_root;
    
    u_int16_t host_serv_port;

    bool use_card_crypt_hw;	// do we use the 4758 hardware for crypto? (or
				// openssl if false)

    bool use_scc_comm;		// do we use SCC sockets for communicating, or
				// UDP if false
};

// main() should call this, or do its own defaults
void init_default_configs ();

extern configs g_configs;

