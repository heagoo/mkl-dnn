/*******************************************************************************
* Copyright 2017-2018 Intel Corporation
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

#ifndef _REORDER_HPP
#define _REORDER_HPP

#include <vector>

#include "mkldnn.h"

#include "common.hpp"
#include "dnn_types.hpp"
#include "mkldnn_common.hpp"
#include "mkldnn_memory.hpp"
#include "perf_report.hpp"

namespace reorder {

using dims_t = std::vector<int64_t>;

enum alg_t { ALG_REF, ALG_BOOT };
alg_t str2alg(const char *str);
const char *alg2str(alg_t alg);

enum flag_t { FLAG_NONE, FLAG_CONV_S8S8, FLAG_GCONV_S8S8 };
flag_t str2flag(const char *str);
const char *flag2str(flag_t flag);

struct dt_conf_s {
    mkldnn_data_type_t dt;
    int min;
    int range;
};
typedef const dt_conf_s *dt_conf_t;

extern const dt_conf_t conf_f32;
extern const dt_conf_t conf_s8;
extern const dt_conf_t conf_u8;
extern const dt_conf_t conf_s32;

dt_conf_t dt2cfg(mkldnn_data_type_t dt);
mkldnn_data_type_t cfg2dt(dt_conf_t cfg);

struct reorder_conf_t {
    dims_t dims;
    mkldnn_format_tag_t tag_in, tag_out;
};

struct q10n_conf_t {
    dt_conf_t conf_in;
    dt_conf_t conf_out;
    /* TODO: add attrs */
    attr_t::scale_t::policy_t policy;
    float scale;
};

struct prb_t {
    prb_t(const reorder_conf_t &r, const dt_conf_t &conf_in,
            const dt_conf_t &conf_out, const attr_t &attr, alg_t alg,
            flag_t oflag, float scale = 0.f)
        : reorder(r)
        , conf_in(conf_in)
        , conf_out(conf_out)
        , attr(attr)
        , alg(alg)
        , oflag(oflag)
        , ops(0) {
        if (scale != 0.f) this->attr.oscale.scale = scale;
        count_ops();
    }

    const reorder_conf_t reorder;
    dt_conf_t conf_in;
    dt_conf_t conf_out;
    attr_t attr;
    alg_t alg;
    flag_t oflag;
    double ops;

    void count_ops() {
        if (ops > 0) return;

        ops = 1;
        for (size_t d = 0; d < reorder.dims.size(); ++d)
            ops *= reorder.dims[d];
    };
};

dims_t str2dims(const char *str);
void dims2str(const dims_t &dims, char *buffer);
void prb2str(const prb_t *p, char *buffer);

struct perf_report_t: public base_perf_report_t {
    perf_report_t(const char *perf_template) :
        base_perf_report_t(perf_template) {}

    virtual ~perf_report_t() {}

    void report(const prb_t *p, const res_t *r, const char *prb_str) {
        p_ = p;
        base_report(r, prb_str);
    }

    virtual void dump_algorithm(char *buf) const override {
        dprint(buf, alg2str(p_->alg));
    }

    virtual void dump_attributes(char *buf) const override {
        if (!p_->attr.is_def())
            attr2str(&p_->attr, buf);
    }

    virtual void dump_data_type(char *buf) const override {
        snprintf(buf, max_dump_len, "%s,%s", dt2str(cfg2dt(p_->conf_in)),
                dt2str(cfg2dt(p_->conf_out)));
    }

    virtual void dump_descriptor_csv(char *buf) const override {
        dims2str(p_->reorder.dims, buf);
    }

    virtual void dump_flags(char *buf) const override {
        dprint(buf, flag2str(p_->oflag));
    }

    virtual void dump_tag(char *buf) const override {
        snprintf(buf, max_dump_len, "%s,%s", tag2str(p_->reorder.tag_in),
                tag2str(p_->reorder.tag_out));
    }

    virtual double ops() const override {
        return p_->ops;
    }

private:
    const prb_t *p_;
};

int doit(const prb_t *p, res_t *res);
int bench(int argc, char **argv);

}

#endif
