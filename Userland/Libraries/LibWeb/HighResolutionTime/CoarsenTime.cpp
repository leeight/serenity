/*
 * Copyright (c) 2022, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Types.h>
#include <LibWeb/HighResolutionTime/CoarsenTime.h>

namespace Web::HighResolutionTime {

static constexpr int COARSEN_RESOLUTION_MS = 100;
static constexpr int DEFAULT_RESOLUTION_MS = 5;

// https://w3c.github.io/hr-time/#dfn-coarsen-time
double coarsen_time(double timestamp, bool cross_origin_isolated_capability)
{
    i64 time_microseconds = static_cast<i64>(timestamp);
    bool is_negative = false;
    if (time_microseconds < 0) {
        is_negative = true;
        time_microseconds = -time_microseconds;
    }

    i64 time_lower_digits = time_microseconds % 10000000000;
    i64 time_upper_digits = time_microseconds - time_lower_digits;
    i32 resolution = cross_origin_isolated_capability
        ? DEFAULT_RESOLUTION_MS
        : COARSEN_RESOLUTION_MS;
    i64 clamped_time = time_lower_digits - time_lower_digits % resolution;
    // Determine if the clamped number should be clamped up, rather than down.
    // The threshold to determine that is a random number smaller than resolution,
    // such that the probability of clamped time being clamped up rather than
    // down is proportional to its distance from the clamped_down time.
    // As such it remains a double, in order to guarantee that distribution,
    // and the clamping's uniformity.
    // double tick_threshold = ThresholdFor(clamped_time, resolution);
    // if (time_lower_digits >= tick_threshold) {
    //     clamped_time += resolution;
    // }

    // Add back the upper digits portion.
    clamped_time += time_upper_digits;

    if (is_negative) {
       clamped_time = -clamped_time;
    }

    return static_cast<double>(clamped_time);
}

}
