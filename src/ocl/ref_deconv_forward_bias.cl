/*******************************************************************************
* Copyright 2019 Intel Corporation
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

#include "ocl/ocl_types.h"

__kernel void ref_deconv_forward_bias(
        __global DATA_T *dst, __global DATA_T *bias) {

    const int mb = get_global_id(1);
    const int g = get_global_id(2) / OC;
    const int oc = get_global_id(2) % OC;
    DATA_T b = bias[g * OC + oc];
    for (int od = 0; od < OD; ++od)
        for (int oh = 0; oh < OH; ++oh)
            for (int ow = 0; ow < OW; ++ow) {
                uint dst_off = DST_OFF(mb, oc, od, oh, ow);
                dst[dst_off] += b;
            }
}
