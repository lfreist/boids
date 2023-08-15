#pragma once

#include <particle/ocl/types.h>

#include <cmath>

template <typename T>
concept CL_VALID_TYPE =
    std::is_same_v<T, cl_float2> || std::is_same_v<T, cl_float3> ||
    std::is_same_v<T, cl_float4>;

template <CL_VALID_TYPE T> class Particles {
  friend class BoidsSimulation;
  template <CL_VALID_TYPE>
  friend class Grid;

public:
  explicit Particles(size_t size)
      : _size(size), _position(new T[size]), _velocity(new T[size]),
        _color(new cl_int3[size]) {}

  Particles(size_t size, T *positions, T *velocities, cl_int3 *color)
      : _size(size), _position(positions), _velocity(velocities), _color(color),
        _color_owned(false), _velocity_owned(false), _position_owned(false) {
    if (_position == nullptr) {
      _position = new T[size];
      _position_owned = true;
    }
    if (_velocity == nullptr) {
      _velocity = new T[size];
      _velocity_owned = true;
    }
    if (_color == nullptr) {
      _color = new cl_int4[size];
      _color_owned = true;
    }
  }

  Particles(Particles<T> &&other)  noexcept {
    if (&other != this) {
      _position = other._position;
      _velocity = other._velocity;
      _color = other._color;
      _position_owned = other._position_owned;
      _velocity_owned = other._velocity_owned;
      _color_owned = other._color_owned;
      _size = other._size;
    }
  }

  Particles& operator=(Particles<T> &&other) noexcept {
    if (&other != this) {
      if (_position_owned)
        delete[] _position;
      if (_velocity_owned)
        delete[] _velocity;
      if (_color_owned)
        delete[] _color;

      _position = other._position;
      _velocity = other._velocity;
      _color = other._color;

      _position_owned = other._position_owned;
      _velocity_owned = other._velocity_owned;
      _color_owned = other._color_owned;

      _size = other._size;

      other._position = nullptr;
      other._velocity = nullptr;
      other._color = nullptr;
      other._size = 0;
    }
    return *this;
  }

  Particles& operator=(const Particles<T>& other) {
    if (this != &other) {
      if (_position_owned)
        delete[] _position;
      if (_velocity_owned)
        delete[] _velocity;
      if (_color_owned)
        delete[] _color;

      _size = other._size;
      if (other._position_owned) {
        _position = new T[_size];
        _position_owned = true;
        std::memcpy(_position, other._position, _size);
      } else {
        _position = other._position;
        _position_owned = false;
      }
      if (other._velocity_owned) {
        _velocity = new T[_size];
        _velocity_owned = true;
        std::memcpy(_velocity, other._velocity, _size);
      } else {
        _velocity = other._velocity;
        _velocity_owned = false;
      }
      if (other._color_owned) {
        _color = new cl_int4[_size];
        _color_owned = true;
        std::memcpy(_color, other._color, _size);
      } else {
        _color = other._color;
        _color_owned = false;
      }
    }
    return *this;
  }

  ~Particles() {
    if (_position_owned)
      delete[] _position;
    if (_velocity_owned)
      delete[] _velocity;
    if (_color_owned)
      delete[] _color;
  }

  void set_random_positions(int x0, int x1, int y0, int y1) {
    std::random_device rd;  // Used to seed the engine
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_int_distribution<int> get_w(x0, x1); // Range from 1 to 100
    std::uniform_int_distribution<int> get_h(y0, y1); // Range from 1 to 100

    for (int i = 0; i < _size; ++i) {
      _position[i].x = get_w(gen);
      _position[i].y = get_h(gen);
    }
  }

  void resize(size_t size) {
    T *old_position = _position;
    T *old_velocity = _velocity;
    cl_int4 *old_color = _color;

    if (_position_owned)
      delete[] _position;
    if (_velocity_owned)
      delete[] _velocity;
    if (_color_owned)
      delete[] _color;

    _position = new T[size];
    _velocity = new T[size];
    _color = new cl_int4[size];

    size_t copy_size = std::min(size, _size);

    std::memcpy(_position, old_position, copy_size);
    std::memcpy(_velocity, old_velocity, copy_size);
    std::memcpy(_color, old_color, copy_size);

    _position_owned = true;
    _velocity_owned = true;
    _color_owned = true;
  }

  T *position_data() { return _position; }
  T *velocity_data() { return _velocity; }
  cl_int4 *color_data() { return _color; }

  void draw(SDL_Renderer *renderer) const {
    for (int i = 0; i < _size; ++i) {
      SDL_SetRenderDrawColor(renderer, _color[i].x, _color[i].y, _color[i].z,
                             _color[i].w);
      SDL_RenderDrawPoint(renderer, _position[i].x, _position[i].y);
    }
  }

  [[nodiscard]] size_t size() const { return _size; }

private:
  size_t _size;

  T *_position;
  T *_velocity;
  cl_int4 *_color;

  bool _position_owned{true};
  bool _velocity_owned{true};
  bool _color_owned{true};
};
