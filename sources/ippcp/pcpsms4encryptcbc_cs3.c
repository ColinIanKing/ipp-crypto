/*******************************************************************************
* Copyright (C) 2013 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the 'License');
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* 
* http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing,
* software distributed under the License is distributed on an 'AS IS' BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions
* and limitations under the License.
* 
*******************************************************************************/

/* 
// 
//  Purpose:
//     Cryptography Primitive.
//     SMS4 encryption/decryption
// 
//  Contents:
//        ippsSMS4EncryptCBC_CS3()
//
*/

#include "owncp.h"
#include "pcpsms4.h"
#include "pcptool.h"
#include "pcpsms4_encrypt_cbc.h"

/*F*
//    Name: ippsSMS4EncryptCBC_CS3
//
// Purpose: SMS4-CBC-CS3 encryption.
//
// Returns:                Reason:
//    ippStsNullPtrErr        pCtx == NULL
//                            pSrc == NULL
//                            pDst == NULL
//                            pIV  == NULL
//    ippStsContextMatchErr   !VALID_SMS4_ID()
//    ippStsLengthErr         len <=MBS_SMS4 (different from CS1 and CS2)
//    ippStsNoErr             no errors
//
// Parameters:
//    pSrc        pointer to the source data buffer
//    pDst        pointer to the target data buffer
//    len         input/output buffer length (in bytes)
//    pCtx        pointer to the SMS4 context
//    pIV         pointer to the initialization vector
//
// Note:
// - CS3 corresponds to definitions specified for Kerberos 3 in RFC2040 and RFC3962
// - C*[n-2] and C[n-1] are unconditionally swapped, even if C*[n-2] is a complete block
// - Therefore, CBC-CS3 is not strictly an extensioin of usual CBC mode
//
*F*/
IPPFUN(IppStatus, ippsSMS4EncryptCBC_CS3,(const Ipp8u* pSrc, Ipp8u* pDst, int len,
                                          const IppsSMS4Spec* pCtx,
                                          const Ipp8u* pIV))
{
   /* test context */
   IPP_BAD_PTR1_RET(pCtx);
   /* test the context ID */
   IPP_BADARG_RET(!VALID_SMS4_ID(pCtx), ippStsContextMatchErr);

   /* test source, target buffers and initialization pointers */
   IPP_BAD_PTR3_RET(pSrc, pIV, pDst);
   /* test stream length */
   IPP_BADARG_RET((len<=MBS_SMS4), ippStsLengthErr);

   {
      int tail = len & (MBS_SMS4-1); /* length of the last partial block */
      if(0==tail)
         tail = MBS_SMS4;
      len -= tail;

      /* encryption of complete blocks */
      cpEncryptSMS4_cbc(pIV, pSrc, pDst, len, pCtx);
      pSrc += len;
      pDst += len;

      {
         __ALIGN16 Ipp8u TMP[2*MBS_SMS4];

         /*
            lastIV     size = MBS_SMS4
            lastEncBlk size = MBS_SMS4
         */

         Ipp8u*     lastIV = TMP;
         Ipp8u* lastEncBlk = TMP + MBS_SMS4;

         int n;

         CopyBlock16(pDst-MBS_SMS4, lastIV);
         CopyBlock16(pDst-MBS_SMS4, lastEncBlk);
         for(n=0; n<tail; n++)
            lastIV[n] ^= pSrc[n];

         /* encrypt last padded block */
         cpSMS4_Cipher(pDst-MBS_SMS4, lastIV, SMS4_RK(pCtx));

         CopyBlock(lastEncBlk, pDst, tail);

         /* clear secret data */
         PurgeBlock(TMP, sizeof(TMP));
      }

      return ippStsNoErr;
   }
}
