/*******************************************************************************
* Copyright 2019-2020 Intel Corporation
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

#ifndef IFMA_RSA_STATUS_H
#define IFMA_RSA_STATUS_H

#include "rsa_ifma_defs.h"

typedef int32u ifma_status;

// error statuses and manipulators
#define IFMA_STATUS_OK                 (0)
#define IFMA_STATUS_MISMATCH_PARAM_ERR (1)
#define IFMA_STATUS_NULL_PARAM_ERR     (2)
#define IFMA_STATUS_LOW_ORDER_ERR      (4)

__INLINE ifma_status IFMA_SET_STS(ifma_status stt, int numb, ifma_status sttVal)
{
   numb &= 7; /* 0 <= numb < 8 */
   stt &= (ifma_status)(~(0xF << (numb*4)));
   return stt |= (sttVal & 0xF) << (numb*4);
}

__INLINE ifma_status IFMA_GET_STS(ifma_status stt, int numb)
{
   return (stt >>(numb*4)) & 0xF;
}
__INLINE ifma_status IFMA_SET_STS_ALL(ifma_status stsVal)
{
   return (stsVal<<4*7) | (stsVal<<4*6) | (stsVal<<4*5) | (stsVal<<4*4)  | (stsVal<<4*3) | (stsVal<<4*2) | (stsVal<<4*1) | stsVal;
}

__INLINE ifma_status IFMA_SET_STS_BY_MASK(ifma_status stt, int8u mask, ifma_status sttVal)
{
   for(int numb=0; numb<8; numb++) {
      ifma_status buf_stt = (0 - ((mask>>numb) &1)) & sttVal;
      stt = IFMA_SET_STS(stt, numb, buf_stt);
   }
   return stt;
}

__INLINE int IFMA_IS_ANY_OK_STS(ifma_status stt)
{
   int ret = IFMA_STATUS_OK==IFMA_GET_STS(stt, 0)
          || IFMA_STATUS_OK==IFMA_GET_STS(stt, 1)
          || IFMA_STATUS_OK==IFMA_GET_STS(stt, 2)
          || IFMA_STATUS_OK==IFMA_GET_STS(stt, 3)
          || IFMA_STATUS_OK==IFMA_GET_STS(stt, 4)
          || IFMA_STATUS_OK==IFMA_GET_STS(stt, 5)
          || IFMA_STATUS_OK==IFMA_GET_STS(stt, 6)
          || IFMA_STATUS_OK==IFMA_GET_STS(stt, 7);
   return ret;
}

#endif /* IFMA_RSA_STATUS_H */
