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

#ifndef _SOFTMAX_HPP
#define _SOFTMAX_HPP

#include "mkldnn.h"

#include "common.hpp"
#include "mkldnn_common.hpp"
#include "mkldnn_memory.hpp"
#include "perf_report.hpp"

namespace softmax {

using dims_t = std::vector<int64_t>;

struct prb_t {
    prb_t(dims_t &dims, dir_t dir, mkldnn_data_type_t dt,
            mkldnn_format_tag_t tag, int axis, int64_t mb = 0)
        : dims(dims), dir(dir), dt(dt), tag(tag), axis(axis) {
        if (mb) this->dims[0] = mb;
    }
    ~prb_t() {}

    dims_t dims;
    dir_t dir;
    mkldnn_data_type_t dt;
    mkldnn_format_tag_t tag;
    int axis;
};

dims_t str2dims(const char *str);
void dims2str(const dims_t &dims, char *buffer);
void prb2str(const prb_t *p, char *buffer, bool canonical = false);

struct perf_report_t: public base_perf_report_t {
    perf_report_t(const char *perf_template) :
        base_perf_report_t(perf_template) {}

    virtual ~perf_report_t() {}

    void report(const prb_t *p, const res_t *r, const char *prb_str) {
        p_ = p;
        base_report(r, prb_str);
    }

    virtual void dump_axis(char *buf) const override {
        dprint(buf, p_->axis);
    }

    virtual void dump_data_type(char *buf) const override {
        dprint(buf, dt2str(p_->dt));
    }

    virtual void dump_descriptor_csv(char *buf) const override {
        dims2str(p_->dims, buf);
    }

    virtual void dump_direction(char *buf) const override {
        dprint(buf, dir2str(p_->dir));
    }

    virtual void dump_tag(char *buf) const override {
        dprint(buf, tag2str(p_->tag));
    }

private:
    const prb_t *p_;
};

inline void map_off_to_mb_ic(const prb_t *p, int64_t off, int64_t &mb,
        int64_t &ic) {
    for (int i = (int)p->dims.size() - 1; i > 1; i--)
        off /= p->dims[i];

    ic = off % p->dims[1]; off /= p->dims[1];
    mb = off % p->dims[0]; off /= p->dims[0];
    assert(off == 0);
}

void compute_ref_fwd(const prb_t *p, const dnn_mem_t &src, dnn_mem_t &dst);
void compute_ref_bwd(const prb_t *p, const dnn_mem_t &dst,
        const dnn_mem_t &diff_dst, dnn_mem_t &diff_src);

int doit(const prb_t *p, res_t *res);
int bench(int argc, char **argv);

}

#endif
