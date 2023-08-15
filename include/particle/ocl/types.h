/**
 * Copyright 2023, Leon Freist (https://github.com/lfreist)
 * Author: Leon Freist <freist.leon@gmail.com>
 *
 * This file is part of kiss-ocl.
 */

#pragma once

#include <missocl/opencl.h>

class Velocity {
public:
  Velocity(float x, float y, float z = 0) : _data({x, y, z}) {}

  [[nodiscard]] cl_float3 cl_data() const { return _data; }
  [[nodiscard]] const float *data() const { return &_data.x; }

  [[nodiscard]] constexpr size_t size() const { return 3; }
  [[nodiscard]] constexpr size_t mem_size() const { return sizeof(_data); }

  float operator[](int index) const {
    switch (index) {
    case 0:
      return _data.x;
    case 1:
      return _data.y;
    case 2:
      return _data.z;
    default:
      throw std::out_of_range("Index '" + std::to_string(index) +
                              "' out of range (0 <= index <= 2).");
    }
  }

  [[nodiscard]] float x() const { return _data.x; }
  [[nodiscard]] float y() const { return _data.y; }
  [[nodiscard]] float z() const { return _data.z; }

  void x(float val) { _data.x = val; }
  void y(float val) { _data.y = val; }
  void z(float val) { _data.z = val; }

  Velocity& operator+=(const Velocity& other) {
    _data.x += other.x();
    _data.y += other.y();
    _data.z += other.z();
    return *this;
  }

  Velocity& operator-=(const Velocity& other) {
    _data.x -= other.x();
    _data.y -= other.y();
    _data.z -= other.z();
    return *this;
  }

  Velocity& operator*=(const Velocity& other) {
    _data.x *= other.x();
    _data.y *= other.y();
    _data.z *= other.z();
    return *this;
  }

  Velocity& operator*=(float val) {
    _data.x *= val;
    _data.y *= val;
    _data.z *= val;
    return *this;
  }

  Velocity& operator/=(float val) {
    _data.x /= val;
    _data.y /= val;
    _data.z /= val;
    return *this;
  }

  friend Velocity operator+(Velocity lhs,
                     const Velocity& rhs) {
    lhs += rhs;
    return lhs;
  }

  friend Velocity operator*(Velocity lhs, float rhs) {
    lhs *= rhs;
    return lhs;
  }

  [[nodiscard]] double length() const {
    auto i = sqrt(_data.x * _data.x + _data.y * _data.y + _data.z * _data.z);
    return i;
  }

private:
  cl_float3 _data;
};

class Position {
public:
  Position(int x, int y, int z = 0) : _data({x, y, z}) {}

  [[nodiscard]] cl_int3 cl_data() const { return _data; }
  [[nodiscard]] const int *data() const { return &_data.x; }

  [[nodiscard]] constexpr size_t size() const { return 3; }
  [[nodiscard]] constexpr size_t mem_size() const { return sizeof(_data); }

  int operator[](int index) const {
    switch (index) {
    case 0:
      return _data.x;
    case 1:
      return _data.y;
    case 2:
      return _data.z;
    default:
      throw std::out_of_range("Index '" + std::to_string(index) +
                              "' out of range (0 <= index <= 2).");
    }
  }

  [[nodiscard]] int x() const { return _data.x; }
  [[nodiscard]] int y() const { return _data.y; }
  [[nodiscard]] int z() const { return _data.z; }

  [[nodiscard]] double length() const {
    return sqrt(_data.x * _data.x + _data.y * _data.y + _data.z * _data.z);
  }

  void x(int val) { _data.x = val; }
  void y(int val) { _data.y = val; }
  void z(int val) { _data.z = val; }

  Position& operator+=(const Position& other) {
    _data.x += other.x();
    _data.y += other.y();
    _data.z += other.z();
    return *this;
  }
  Position& operator-=(const Position& other) {
    _data.x -= other.x();
    _data.y -= other.y();
    _data.z -= other.z();
    return *this;
  }

  Position& operator*=(const Position& other) {
    _data.x *= other.x();
    _data.y *= other.y();
    _data.z *= other.z();
    return *this;
  }

  Position& operator*=(float other) {
    _data.x = static_cast<cl_int>(other * static_cast<float>(_data.x));
    _data.y = static_cast<cl_int>(other * static_cast<float>(_data.y));
    _data.z = static_cast<cl_int>(other * static_cast<float>(_data.z));
    return *this;
  }

  Position& operator/=(float val) {
    _data.x = static_cast<cl_int>(static_cast<float>(_data.x) / val);
    _data.y = static_cast<cl_int>(static_cast<float>(_data.y) / val);
    _data.z = static_cast<cl_int>(static_cast<float>(_data.z) / val);
    return *this;
  }

  friend Position operator+(Position lhs,
                            const Position& rhs) {
    lhs += rhs;
    return lhs;
  }

  friend Position operator-(Position lhs, const Position& rhs) {
    lhs -= rhs;
    return lhs;
  }

  friend Position operator*(Position lhs, float rhs) {
    lhs *= rhs;
    return lhs;
  }

  friend Position operator/(Position lhs, float rhs) {
    lhs /= rhs;
    return lhs;
  }

private:
  cl_int3 _data;
};