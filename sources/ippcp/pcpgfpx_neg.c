/*******************************************************************************
* Copyright 2010-2020 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/*
//     Intel(R) Integrated Performance Primitives. Cryptography Primitives.
//     Internal operations over GF(p) extension.
// 
//     Context:
//        cpGFpxNeg()
//
*/

#include "owncp.h"
#include "pcpbnumisc.h"
#include "pcpgfpxstuff.h"
#include "gsscramble.h"

IPP_OWN_DEFN (BNU_CHUNK_T*, cpGFpxNeg, (BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, gsModEngine* pGFEx))
{
   gsModEngine* pBasicGFE = cpGFpBasic(pGFEx);
   int basicElemLen = GFP_FELEN(pBasicGFE);
   int basicDeg = cpGFpBasicDegreeExtension(pGFEx);

   BNU_CHUNK_T* pTmp = pR;
   int deg;
   for(deg=0; deg<basicDeg; deg++) {
      GFP_METHOD(pBasicGFE)->neg(pTmp, pA, pBasicGFE);
      pTmp += basicElemLen;
      pA += basicElemLen;
   }
   return pR;
}
