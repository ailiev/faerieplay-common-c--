/*
 * ** PIR Private Directory Service prototype
 * ** Copyright (C) 2003 Alexander Iliev <iliev@nimbus.dartmouth.edu>
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
