#include <common/consts.h>

#include "configs.h"
#include "consts.h"

// instantiate the global configs object, with reasonable defaults

configs g_configs;


void init_default_configs () {
    g_configs.crypto_dir = CARD_CRYPTO_DIR;
    g_configs.host_serv_port = PIR_HOSTSERV_PORT;
    g_configs.use_card_crypt_hw = false;
    g_configs.use_scc_comm = true;
}
