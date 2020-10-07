/*******************************************************************************
* Copyright 2017-2020 Intel Corporation
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
//
//  Purpose:
//     Cryptography Primitive.
//     Internal Definitions of AVX512 Montgomery Exp
//
*/
#include "owncp.h"

#if (_IPP32E>=_IPP32E_K0)

#include "pcpbnuimpl.h"
#include "pcpngmontexpstuff.h"

#define RSA_AVX512_MIN_BITSIZE  (1024)
#define RSA_AVX512_MAX_BITSIZE  (13*1024)

#define NORM_DIGSIZE_AVX512 (BITSIZE(Ipp64u))
#define NORM_BASE_AVX512    ((Ipp64u)1<<NORM_DIGSIZE_AVX512)
#define NORM_MASK_AVX512    (NORM_BASE_AVX512-1)

#define EXP_DIGIT_SIZE_AVX512 (52)
#define EXP_DIGIT_BASE_AVX512 (1<<EXP_DIGIT_SIZE_AVX512)
#define EXP_DIGIT_MASK_AVX512 ((Ipp64u)0xFFFFFFFFFFFFF)

/* number of digits */
__INLINE int cpDigitNum_avx512(int bitSize, int digSize)
{ return (bitSize + digSize-1)/digSize; }

/* number of "EXP_DIGIT_SIZE_AVX512" chunks in "bitSize" bit string matched for AMM */
__INLINE cpSize numofVariable_avx512(int modulusBits)
{
   cpSize ammBitSize = 2 + cpDigitNum_avx512(modulusBits, BITSIZE(BNU_CHUNK_T)) * BITSIZE(BNU_CHUNK_T);
   cpSize redNum = cpDigitNum_avx512(ammBitSize, EXP_DIGIT_SIZE_AVX512);
   return redNum;
}

/* buffer corresponding to numofVariable_avx2() */
/* cpMontExp_avx512_BufferSize() */
__INLINE int numofVariableBuff_avx512(int len)
{
   int tail = len%8;
   if(0==tail) tail = 8;
   return len + (8-tail);
}

/* exponentiation buffer size */
#define gsMontExpBinBuffer_avx512 OWNAPI(gsMontExpBinBuffer_avx512)
   IPP_OWN_DECL (cpSize, gsMontExpBinBuffer_avx512, (int modulusBits))
#define gsMontExpWinBuffer_avx512 OWNAPI(gsMontExpWinBuffer_avx512)
   IPP_OWN_DECL (cpSize, gsMontExpWinBuffer_avx512, (int modulusBits))
#define gsMontDExpWinBuffer_avx512 OWNAPI(gsMontDExpWinBuffer_avx512)
   IPP_OWN_DECL (cpSize, gsMontDExpWinBuffer_avx512, (int modulusBits))

/* exponentiations */
#define gsMontExpBin_BNU_avx512 OWNAPI(gsMontExpBin_BNU_avx512)
   IPP_OWN_DECL (cpSize, gsMontExpBin_BNU_avx512, (BNU_CHUNK_T* dataY, const BNU_CHUNK_T* dataX, cpSize nsX, const BNU_CHUNK_T* dataE, cpSize nsE, gsModEngine* pMont, BNU_CHUNK_T* pBuffer))
#define gsMontExpWin_BNU_avx512 OWNAPI(gsMontExpWin_BNU_avx512)
   IPP_OWN_DECL (cpSize, gsMontExpWin_BNU_avx512, (BNU_CHUNK_T* dataY, const BNU_CHUNK_T* dataX, cpSize nsX, const BNU_CHUNK_T* dataE, cpSize nsE, gsModEngine* pMont, BNU_CHUNK_T* pBuffer))
#define gsMontExpBin_BNU_sscm_avx512 OWNAPI(gsMontExpBin_BNU_sscm_avx512)
   IPP_OWN_DECL (cpSize, gsMontExpBin_BNU_sscm_avx512, (BNU_CHUNK_T* dataY, const BNU_CHUNK_T* dataX, cpSize nsX, const BNU_CHUNK_T* dataE, cpSize nsE, gsModEngine* pMont, BNU_CHUNK_T* pBuffer))
#define gsMontExpWin_BNU_sscm_avx512 OWNAPI(gsMontExpWin_BNU_sscm_avx512)
   IPP_OWN_DECL (cpSize, gsMontExpWin_BNU_sscm_avx512, (BNU_CHUNK_T* dataY, const BNU_CHUNK_T* dataX, cpSize nsX, const BNU_CHUNK_T* dataE, cpSize nsE, gsModEngine* pMont, BNU_CHUNK_T* pBuffer))

/* dual exponentiation */
#define gsMontDExp2x1024_Win_BNU_sscm_avx512 OWNAPI(gsMontDExp2x1024_Win_BNU_sscm_avx512)
   IPP_OWN_DECL (cpSize, gsMontDExp2x1024_Win_BNU_sscm_avx512, (BNU_CHUNK_T* dataY[2], const BNU_CHUNK_T* dataX[2], cpSize nsX[2], const BNU_CHUNK_T* dataE[2], gsModEngine* pMont[2], BNU_CHUNK_T* pBuffer))

#endif /* _IPP32E_K0 */
