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

#include <stdlib.h>
#include <stdio.h>

#include "mkldnn.h"

#include "mkldnn_common.hpp"
#include "mkldnn_memory.hpp"
#include "parser.hpp"

#include "softmax/softmax.hpp"

namespace softmax {

std::vector<dir_t> dir {FWD_D};
std::vector<mkldnn_data_type_t> dt {mkldnn_f32};
std::vector<mkldnn_format_tag_t> tag {mkldnn_nchw};
std::vector<int> axis {1};
std::vector<int64_t> mb {0};

dims_t dims;
bool allow_unimpl = false;
const char *perf_template_csv =
    "perf,%engine%,%dir%,%dt%,%tag%,%axis%,%DESC%,%-time%,%0time%";
const char *perf_template_def = "perf,%engine%,%desc%,%-time%,%0time%";
const char *perf_template = perf_template_def;

void reset_parameters() {
    dir = {FWD_D};
    dt = {mkldnn_f32};
    tag = {mkldnn_nchw};
    axis = {1};
    mb = {0};
    allow_unimpl = false;
}

void check_correctness() {
    for (const auto &i_dir: dir)
    for (const auto &i_dt: dt)
    for (const auto &i_tag: tag)
    for (const auto &i_axis: axis)
    for (const auto &i_mb: mb) {
        const prb_t p(dims, i_dir, i_dt, i_tag, i_axis, i_mb);
        char pstr[max_prb_len];
        prb2str(&p, pstr);

        res_t res{};
        const int status = doit(&p, &res);

        bool want_perf_report = false;
        parse_result(res, want_perf_report, allow_unimpl, status, pstr);

        if (want_perf_report && bench_mode & PERF) {
            perf_report_t pr(perf_template);
            pr.report(&p, &res, pstr);
        }

        benchdnn_stat.tests++;
    }
}

int bench(int argc, char **argv) {
    using namespace parser;
    for (; argc > 0; --argc, ++argv) {
        if (parse_bench_settings(argv[0]));
        else if (parse_batch(bench, argv[0]));
        else if (parse_dir(dir, argv[0]));
        else if (parse_dt(dt, argv[0]));
        else if (parse_tag(tag, argv[0]));
        else if (parse_axis(axis, argv[0]));
        else if (parse_mb(mb, argv[0]));
        else if (parse_allow_unimpl(allow_unimpl, argv[0]));
        else if (parse_perf_template(perf_template, perf_template_def,
                    perf_template_csv, argv[0]));
        else if (parse_reset(reset_parameters, argv[0]));
        else {
            catch_unknown_options(argv[0], "softmax");

            dims = str2dims(argv[0]);
            check_correctness();
        }
    }

    return OK;
}
}
