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
//     EC over GF(p^m) definitinons
// 
//     Context:
//        ippsGFpECGetInfo_GF()
//
*/

#include "owncp.h"
#include "pcpeccp.h"


/*F*
// Name: ippsGFpECGetInfo_GF
//
// Purpose: Returns info regarding underlying GF
//
// Returns:                   Reason:
//    ippStsNullPtrErr              NULL == pEC
//                                  NULL == pInfo
//
//    ippStsContextMatchErr         invalid pEC->idCtx
//
//    ippStsNoErr                   no error
//
// Parameters:
//    pInfo      Pointer to the info structure
//    pEC        Pointer to the context of the elliptic curve being initialized.
//
*F*/
IPPFUN(IppStatus, ippsGFpECGetInfo_GF,(IppsGFpInfo* pInfo, const IppsGFpECState* pEC))
{
   IPP_BAD_PTR2_RET(pInfo, pEC);
   IPP_BADARG_RET( !VALID_ECP_ID(pEC), ippStsContextMatchErr );

   return ippsGFpGetInfo(pInfo, ECP_GFP(pEC));
   #if 0
   {
      IppsGFpState*  pGF = ECP_GFP(pEC);
      gsModEngine* pGFpx = GFP_PMA(pGF);     /* current */
      gsModEngine* pGFp  = cpGFpBasic(pGFpx); /* basic */
      pInfo->parentGFdegree = MOD_EXTDEG(pGFpx);               /* parent extension */
      pInfo->basicGFdegree = cpGFpBasicDegreeExtension(pGFpx); /* total basic extension */
      pInfo->basicElmBitSize = GFP_FEBITLEN(pGFp);             /* basic bitsise */

      return ippStsNoErr;
   }
   #endif
}
