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
//     RSASSA-PSS
// 
//     Signatire Scheme with Appendix Signatute Generation
//     (Ppobabilistic Signature Scheme)
// 
//  Contents:
//        ippsRSAVerify_PSS()
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcpngrsa.h"
#include "pcphash.h"
#include "pcptool.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/*F*
// Name: ippsRSAVerify_PSS
//
// Purpose: Performs Signature Verification according to RSASSA-PSS
//
// Returns:                   Reason:
//    ippStsNotSupportedModeErr  invalid hashAlg value
//
//    ippStsNullPtrErr           NULL == pMsg
//                               NULL == pSign
//                               NULL == pIsValid
//                               NULL == pKey
//                               NULL == pBuffer
//
//    ippStsLengthErr            msgLen<0
//                               RSAsize <=hashLen +2
//
//    ippStsContextMatchErr      !RSA_PUB_KEY_VALID_ID()
//
//    ippStsIncompleteContextErr public key is not set up
//
//    ippStsNoErr                no error
//
// Parameters:
//    pMsg        pointer to the message to be verified
//    msgLen      length of the message
//    pSign       pointer to the signature string of the RSA length
//    pIsValid    pointer to the verification result
//    pKey        pointer to the RSA public key context
//    hashAlg     hash ID
//    pBuffer     pointer to scratch buffer
*F*/
IPPFUN(IppStatus, ippsRSAVerify_PSS,(const Ipp8u* pMsg,  int msgLen,
                                     const Ipp8u* pSign,
                                           int* pIsValid,
                                     const IppsRSAPublicKeyState*  pKey,
                                           IppHashAlgId hashAlg,
                                           Ipp8u* pScratchBuffer))
{
   /* test hash algorithm ID */
   hashAlg = cpValidHashAlg(hashAlg);
   IPP_BADARG_RET(ippHashAlg_Unknown==hashAlg, ippStsNotSupportedModeErr);

   /* test message length */
   IPP_BADARG_RET((msgLen<0), ippStsLengthErr);
   /* test message pointer */
   IPP_BADARG_RET((msgLen && !pMsg), ippStsNullPtrErr);

   /* test data pointer */
   IPP_BAD_PTR2_RET(pSign, pIsValid);

   /* test public key context */
   IPP_BAD_PTR2_RET(pKey, pScratchBuffer);
   IPP_BADARG_RET(!RSA_PUB_KEY_VALID_ID(pKey), ippStsContextMatchErr);
   IPP_BADARG_RET(!RSA_PUB_KEY_IS_SET(pKey), ippStsIncompleteContextErr);

   {
      Ipp8u hashMsg[MAX_HASH_SIZE];

      /* hash length */
      int hashLen = cpHashSize(hashAlg);

      /* size of RSA modulus in bytes and chunks */
      cpSize rsaBits = RSA_PUB_KEY_BITSIZE_N(pKey);
      cpSize k = BITS2WORD8_SIZE(rsaBits);
      cpSize nsN = BITS_BNU_CHUNK(rsaBits);

      /* align buffer */
      BNU_CHUNK_T* pBuffer = (BNU_CHUNK_T*)(IPP_ALIGNED_PTR(pScratchBuffer, (int)sizeof(BNU_CHUNK_T)) );

      /* temporary BNs */
      __ALIGN8 IppsBigNumState bnC;
      __ALIGN8 IppsBigNumState bnP;

      /* message presentative size */
      int emBits = rsaBits-1;
      int emLen  = BITS2WORD8_SIZE(emBits);

      /* test size consistence */
      if(k <= (hashLen+2))
         IPP_ERROR_RET(ippStsLengthErr);

      /* compute hash of the message */
      ippsHashMessage(pMsg, msgLen, hashMsg, hashAlg);

      /* make BNs */
      BN_Make(pBuffer, pBuffer+nsN+1, nsN, &bnC);
      pBuffer += (nsN+1)*2;
      BN_Make(pBuffer, pBuffer+nsN+1, nsN, &bnP);
      pBuffer += (nsN+1)*2;

      /*
      // public-key operation
      */
      ippsSetOctString_BN(pSign, k, &bnP);
      gsRSApub_cipher(&bnC, &bnP, pKey, pBuffer);

      *pIsValid = 0;
      /*
      // EMSA-PSS verification
      */
      {
         /* convert BN into octet string EM
         // EM = maskedDB || H || 0xBC
         */
         Ipp8u* pEM = (Ipp8u*)BN_BUFFER(&bnC);
         ippsGetOctString_BN(pEM, emLen, &bnC);

         /* test last byte and top of (8*emLen-emBits) bits */
         if(0xBC==pEM[emLen-1] && 0x00==(pEM[0] >>(8-(8*emLen-emBits)))) {
            int psLen;
            Ipp8u* pM = (Ipp8u*)BN_NUMBER(&bnP);

            /* pointers to the EM fields */
            int dbLen = emLen-hashLen-1;
            Ipp8u* pDB = pEM;
            Ipp8u* pH = pEM+dbLen;

            /* recover DB = maskedDB ^ MGF(H) */
            ippsMGF(pH, hashLen, pM, dbLen, hashAlg);
            XorBlock(pDB, pM, pDB, dbLen);

            /* make sure that top 8*emLen-emBits bits are clear */
            pDB[0] &= MAKEMASK32(8-8*emLen+emBits);

            /* skip over padding string (PS) */
            for(psLen=0; psLen<dbLen; psLen++)
               if(pDB[psLen])
                  break;

            /* and test non-zero octet */
            if(psLen<(dbLen) && 0x01==pEM[psLen]) {

               int saltLen = dbLen-1-psLen;

               /* construct message M'
               // M' = (00 00 00 00 00 00 00 00) || mHash || salt
               // where:
               //    mHash = HASH(pMsg)
               */
               PadBlock(0, pM, 8);
               CopyBlock(hashMsg, pM+8, hashLen);
               CopyBlock(pDB+psLen+1, pM+8+hashLen, saltLen);

               /* H' = HASH(M') */
               ippsHashMessage(pM, 8+hashLen+saltLen, pM, hashAlg);

               /* compare H ~ H' */
               *pIsValid = EquBlock(pH, pM, hashLen);
            }
         }
      }

      return ippStsNoErr;
   }
}
