#ifndef CBC_MAC_H_
#define CBC_MAC_H_
#include <openssl/evp.h>
#include <stdlib.h>

#define CBCMAC_MAX_BYTES 64


#ifdef  __cplusplus
extern "C" {
#endif


typedef struct CBCMAC_CTX_st
{
  EVP_CIPHER_CTX cctx;
  char cbcstate[CBCMAC_MAX_BYTES];
  char workspace[CBCMAC_MAX_BYTES];
  short worklen;
}
CBCMAC_CTX;

int CBCMAC_Init (CBCMAC_CTX * mctx, const EVP_CIPHER * c, const unsigned char *k);
int CBCMAC_Update (CBCMAC_CTX * mctx, const char *data, int len);
int CBCMAC_Final (CBCMAC_CTX * mctx, unsigned char *out, int *outl);
int CBCMAC (const EVP_CIPHER * c, const unsigned char *key, int key_len,
	    unsigned char *str, int sz, unsigned char *out, int *outlen);


#ifdef  __cplusplus
} /* extern "C" */
#endif

#endif
