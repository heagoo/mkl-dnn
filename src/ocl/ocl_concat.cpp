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

#include "ocl/ocl_concat_pd.hpp"
#include "ocl/ocl_engine.hpp"
#include "ocl/ref_concat.hpp"

namespace mkldnn {
namespace impl {
namespace ocl {

using cpd_create_f = mkldnn::impl::engine_t::concat_primitive_desc_create_f;

namespace {
#define INSTANCE(...) __VA_ARGS__::pd_t::create
static const cpd_create_f ocl_concat_impl_list[] = {
    INSTANCE(ref_concat_t),
    nullptr,
};
#undef INSTANCE
} // namespace

const cpd_create_f *ocl_engine_t::get_concat_implementation_list() const {
    return ocl_concat_impl_list;
}

} // namespace ocl
} // namespace impl
} // namespace mkldnn
