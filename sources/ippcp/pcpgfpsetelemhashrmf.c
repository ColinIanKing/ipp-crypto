/*******************************************************************************
* Copyright (C) 2018 Intel Corporation
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
//     Intel(R) Integrated Performance Primitives. Cryptography Primitives.
//     Operations over GF(p).
// 
//     Context:
//        ippsGFpSetElementHash_rmf
// 
*/
#include "owndefs.h"
#include "owncp.h"

#include "pcpgfpstuff.h"
#include "pcpgfpxstuff.h"
#include "pcphash.h"
#include "pcphash_rmf.h"
#include "pcptool.h"


/*F*
// Name: ippsGFpSetElementHash_rmf
//
// Purpose: Set GF Element Hash of the Message
//
// Returns:                   Reason:
//    ippStsNullPtrErr           NULL == pGFp
//                               NULL == pElm
//                               NULL == pMsg if msgLen>0
//                               NULL = pMethod
//
//    ippStsContextMatchErr      invalid pGFp->idCtx
//                               invalid pElm->idCtx
//
//    ippStsOutOfRangeErr        GFPE_ROOM() != GFP_FELEN()
//
//    ippStsBadArgErr            !GFP_IS_BASIC(pGFE)
//
//    ippStsLengthErr            msgLen<0
//
//    ippStsNoErr                no error
//
// Parameters:
//    pMsg     pointer to the message is being hashed
//    msgLen   length of the message above
//    pElm     pointer to Finite Field Element context
//    pGFp     pointer to Finite Field context
//    pMethod  pointer to hash method
*F*/

IPPFUN(IppStatus, ippsGFpSetElementHash_rmf,(const Ipp8u* pMsg, int msgLen, IppsGFpElement* pElm, IppsGFpState* pGFp, const IppsHashMethod* pMethod))
{
   /* test method pointer */
   IPP_BAD_PTR1_RET(pMethod);

   /* test message length and pointer */
   IPP_BADARG_RET((msgLen<0), ippStsLengthErr);
   IPP_BADARG_RET((msgLen && !pMsg), ippStsNullPtrErr);

   IPP_BAD_PTR2_RET(pElm, pGFp);
   IPP_BADARG_RET( !GFP_VALID_ID(pGFp), ippStsContextMatchErr);
   IPP_BADARG_RET( !GFPE_VALID_ID(pElm), ippStsContextMatchErr);
   {
      gsModEngine* pGFE = GFP_PMA(pGFp);
      IPP_BADARG_RET( !GFP_IS_BASIC(pGFE), ippStsBadArgErr);
      IPP_BADARG_RET( GFPE_ROOM(pElm)!=GFP_FELEN(pGFE), ippStsOutOfRangeErr);

      {
         Ipp8u md[MAX_HASH_SIZE];
         BNU_CHUNK_T hashVal[(MAX_HASH_SIZE*8)/BITSIZE(BNU_CHUNK_T)+1]; /* +1 to meet cpMod_BNU() implementation specific */
         IppStatus sts = ippsHashMessage_rmf(pMsg, msgLen, md, pMethod);

         if(ippStsNoErr==sts) {
            int elemLen = GFP_FELEN(pGFE);
            int hashLen = pMethod->hashLen;
            int hashValLen = cpFromOctStr_BNU(hashVal, md, hashLen);
            hashValLen = cpMod_BNU(hashVal, hashValLen, GFP_MODULUS(pGFE), elemLen);
            cpGFpSet(GFPE_DATA(pElm), hashVal, hashValLen, pGFE);
         }

         return sts;
      }
   }
}
