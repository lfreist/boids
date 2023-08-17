/**
 * Copyright 2023, Leon Freist (https://github.com/lfreist)
 * Author: Leon Freist <freist.leon@gmail.com>
 *
 * This file is part of kiss-ocl.
 */

#pragma once

#include <missocl/opencl.h>
#include <missocl/ocl_syntax.h>

namespace kernel {

KERNEL_CODE(boids2d,
__kernel void update_boids(__global float3* position,
                           __global float3* velocity,
                           __global int2* start_end_cell,
                           )

)

}