/**
 * Copyright 2023, Leon Freist (https://github.com/lfreist)
 * Author: Leon Freist <freist.leon@gmail.com>
 *
 * This file is part of kiss-ocl.
 */

#pragma once

#include <chrono>
#include <concepts>
#include <missocl/opencl.h>

typedef std::chrono::duration<double> Duration;

typedef cl_float2 Space2D;
typedef cl_float3 Space3D;

template <typename T>
concept Dimension =
    std::is_same_v<T, Space2D> || std::is_same_v<T, Space3D>;

template <typename T>
concept CL_VALID_TYPE =
    std::is_same_v<T, cl_float2> || std::is_same_v<T, cl_float3> ||
    std::is_same_v<T, cl_float4>;


enum class ENGINE { CPU, OPENCL };

enum class BORDER { REFLECTIVE, TOROIDAL, RESET };

struct Space {
  cl_int2 position;
  cl_int2 size;

  [[nodiscard]] size_t height() const { return size.x; }
  [[nodiscard]] size_t width() const { return size.y; }
};
