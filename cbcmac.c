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

#include "cbcmac.h"

// FIXME: sasho: very poor error handling! improve a bit

int
CBCMAC_Init (CBCMAC_CTX * mctx, const EVP_CIPHER * c, const unsigned char *k)
{
  int i, bl;

  EVP_EncryptInit_ex (&(mctx->cctx), c, NULL, (unsigned char *) k, 0);

#if 0
  fprintf (stderr, "Key size = %d\n",
	   EVP_CIPHER_CTX_key_length (&(mctx->cctx)));
#endif
  
  if (EVP_CIPHER_CTX_mode (&(mctx->cctx)) != EVP_CIPH_ECB_MODE)
    return -1;
  mctx->worklen = 0;
  bl = EVP_CIPHER_CTX_block_size (&(mctx->cctx));
  for (i = 0; i < bl; i++)
    mctx->cbcstate[i] = 0;
  return 0;
}

/* We hand implement CBC-mode because of the requirements for the last block,
* and to avoid dynamic memory allocation.
*/
int
CBCMAC_Update (CBCMAC_CTX * mctx, const char *data, int len)
{
  int bl, i, n = 0, outl;

  bl = EVP_CIPHER_CTX_block_size (&(mctx->cctx));

  if (mctx->worklen)
    {
      n = bl - mctx->worklen;
      if (n > len)		/* Not enough bytes passed in to fill block buffer. */
	{
	  for (i = 0; i < len; i++)
	    mctx->workspace[mctx->worklen + i] = data[i];
	  mctx->worklen += len;
	  return 0;
	}
      else
	{
	  for (i = 0; i < n; i++)
	    mctx->workspace[mctx->worklen + i] = data[i] ^ mctx->cbcstate[i];
	  EVP_EncryptUpdate (&(mctx->cctx), mctx->cbcstate, &outl,
			     mctx->workspace, bl);
	}
    }
  while (n < len)
    {
      for (i = 0; i < bl; i++)
	mctx->workspace[i] = data[n + i] ^ mctx->cbcstate[i];
      n = n + bl;
      EVP_EncryptUpdate (&(mctx->cctx), mctx->cbcstate, &outl,
			 mctx->workspace, bl);
    }
  mctx->worklen = len - n;
  for (i = 0; i < mctx->worklen; i++)
    mctx->workspace[i] = data[n + i];
  return 0;
}

int
CBCMAC_Final (CBCMAC_CTX * mctx, unsigned char *out, int *outl)
{
  int i, bl = EVP_CIPHER_CTX_block_size (&(mctx->cctx));

/* Pad with null bytes if necessary. In reality, we just copy in the
* CBC state, since x ^ 0 = x.
*/
  if (mctx->worklen)
    {
      for (i = mctx->worklen; i < bl; i++)
	mctx->workspace[i] = mctx->cbcstate[i];
      EVP_EncryptUpdate (&(mctx->cctx), out, outl, mctx->workspace, bl);
    }
  else
    {
      for (i = 0; i < bl; i++)
	out[i] = mctx->cbcstate[i];
      *outl = bl;
    }
  return 0;
}

#if 0
int
CBCMAC (const EVP_CIPHER * c, const unsigned char *key, int key_len, unsigned char *str,
	int sz, unsigned char *out, int *outlen)
{
  CBCMAC_CTX x;
  int e;

  if ((e = CBCMAC_Init (&x, c, key)))
    return e;
  if ((e = CBCMAC_Update (&x, str, sz)))
    return e;
  e = CBCMAC_Final (&x, out, outlen);

  /* sasho: this is initialized in CBCMAC_Init, but i think not cleaning-up may
  leak memory
  yes! this was the leak!
  */
  EVP_CIPHER_CTX_cleanup (&(x.cctx));

  return e;
}
#endif
