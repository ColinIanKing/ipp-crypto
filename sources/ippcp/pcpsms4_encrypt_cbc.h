/*******************************************************************************
* Copyright 2013-2020 Intel Corporation
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
//     SMS4 encryption/decryption
// 
//  Contents:
//        cpEncryptSMS4_cbc()
//
*/

#include "owncp.h"

#if !defined _PCP_SMS4_ENCRYPT_CBC_H
#define _PCP_SMS4_ENCRYPT_CBC_H

#define cpEncryptSMS4_cbc OWNAPI(cpEncryptSMS4_cbc)
    IPP_OWN_DECL (void, cpEncryptSMS4_cbc, (const Ipp8u* pIV, const Ipp8u* pSrc, Ipp8u* pDst, int dataLen, const IppsSMS4Spec* pCtx))

#endif /* #if !defined _PCP_SMS4_ENCRYPT_CBC_H */
