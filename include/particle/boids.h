//
// Created by leon- on 11/08/2023.
//

#pragma once

#include <cmath>
#include <vector>

#include <particle/grid.h>
#include <particle/particle.h>

#include <particle/utils/thread_pool.h>

class BoidsSimulation {
  friend class Framework;
public:
  enum class Border { REFLECTIVE, TOROIDAL, RESET };

  struct Space {
    int top = 0;
    int bottom = 300;
    int left = 0;
    int right = 700;

    [[nodiscard]] int height() const { return bottom - top; }
    int width() const { return right - left; }
  };

  BoidsSimulation(size_t num_particles)
      : _particles(num_particles), _grid(_space.width(), _space.height(), 20),
        _thread_pool(std::thread::hardware_concurrency()){};

  void set_particles(Particles<cl_float2> particles) {
    _particles = std::move(particles);
  }
  void set_particles(const Particles<cl_float2> &particles) {
    _particles = particles;
  }

  void update() {
    _grid.update(_particles);
    updateBoids();
  }

  void updateBoids() {
    auto apply_boids_rules = [this](int beg, int end) {
      for (size_t i = beg; i < end; ++i) {
        cl_float2 sep = separate(i);
        cl_float2 ali = align(i);
        cl_float2 coh = cohere(i);

        _particles._velocity[i].x += sep.x + ali.x + coh.x;
        _particles._velocity[i].y += sep.y + ali.y + coh.y;

        double speed =
            std::sqrt(_particles._velocity[i].x * _particles._velocity[i].x +
                      _particles._velocity[i].y * _particles._velocity[i].y);

        if (speed > _max_speed) {
          _particles._velocity[i].x = (_particles._velocity[i].x / speed) * _max_speed;
          _particles._velocity[i].y = (_particles._velocity[i].y / speed) * _max_speed;
        }
      }
    };
    _thread_pool.push_loop(_particles.size(), apply_boids_rules);
    _thread_pool.wait_for_tasks();

    auto border_boids = _grid.get_at_border();
    std::vector<size_t> bb(border_boids.begin(), border_boids.end());
    auto border_collision = [this, &bb](auto beg, auto end) {
      for (int i = beg; i < end; ++i) {
        switch (_border) {
        case Border::REFLECTIVE:
          reflection(bb[i]);
          break;
        case Border::TOROIDAL:
          toroid(bb[i]);
          break;
        default:
          reflection(bb[i]);
          break;
        }
      }
    };
    _thread_pool.push_loop(bb.size(), border_collision);
    _thread_pool.wait_for_tasks();

    auto move_boids = [this](int beg, int end) {
      for (int i = beg; i < end; ++i) {
        _particles._position[i].x += _particles._velocity[i].x;
        _particles._position[i].y += _particles._velocity[i].y;
        _particles._color[i].x = (_particles._velocity[i].x / _max_speed) * 255;
        _particles._color[i].y = (_particles._velocity[i].y / _max_speed) * 255;
        _particles._color[i].w = 255;
      }
    };

    _thread_pool.push_loop(_particles.size(), move_boids);
    _thread_pool.wait_for_tasks();
  }

  cl_float2 separate(size_t index) {
    cl_float2 steer{0, 0};
    int count = 0;
    int x = _particles.position_data()[index].x / _grid.grid_size();
    int y = _particles.position_data()[index].y / _grid.grid_size();
    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        int n_x = x + dx;
        int n_y = y + dy;
        if (n_x >= 0 && n_x < _grid.x_size() && n_y >= 0 &&
            n_y < _grid.y_size()) {
          for (size_t i : _grid._grid[n_x][n_y]) {
            double distance = compute_dist(_particles._position[index],
                                           _particles._position[i]);
            if (distance > 0 && distance < _separation_radius) {
              // Calculate the vector pointing away from other boids
              double diffX = _particles._position[index].x - _particles._position[i].x;
              double diffY = _particles._position[index].y - _particles._position[i].y;
              diffX /= distance;
              diffY /= distance;

              steer.x += diffX;
              steer.y += diffY;
              ++count;
            }
          }
        }
      }
    }
    if (count > 0) {
      steer.x /= count;
      steer.y /= count;
      double steerLength = std::sqrt(steer.x * steer.x + steer.y * steer.y);
      if (steerLength > 0) {
        steer.x /= steerLength;
        steer.y /= steerLength;
      }
    }
    return steer;
  }

  cl_float2 align(size_t index) {
    cl_float2 average{0, 0};
    int count = 0;
    int x = _particles._position[index].x / _grid.grid_size();
    int y = _particles._position[index].y / _grid.grid_size();
    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        int n_x = x + dx;
        int n_y = y + dy;
        if (n_x >= 0 && n_x < _grid.x_size() && n_y >= 0 &&
            n_y < _grid.y_size()) {
          for (size_t i : _grid._grid[n_x][n_y]) {
            double distance = compute_dist(_particles._position[index], _particles._position[i]);
            if (distance > 0 && distance < _alignment_radius) {
              average.x += _particles._velocity[i].x;
              average.y += _particles._velocity[i].y;
              ++count;
            }
          }
        }
      }
    }
    if (count > 0) {
      average.x /= count;
      average.y /= count;
      double averageLength = std::sqrt(average.x * average.x + average.y * average.y);
      if (averageLength > 0) {
        average.x /= averageLength;
        average.y /= averageLength;
      }
    }
    return average;
  }

  cl_float2 cohere(size_t index) {
    cl_float2 center{0, 0};
    cl_float2 steer{0, 0};
    int count = 0;

    int x = _particles._position[index].x / _grid.grid_size();
    int y = _particles._position[index].y / _grid.grid_size();
    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        int n_x = x + dx;
        int n_y = y + dy;
        if (n_x >= 0 && n_x < _grid.x_size() && n_y >= 0 &&
            n_y < _grid.y_size()) {
          for (size_t i : _grid._grid[n_x][n_y]) {
            double distance = compute_dist(_particles._position[index], _particles._position[i]);
            if (distance > 0 && distance < _cohesion_radius) {
              center.x += _particles._position[i].x;
              center.y += _particles._position[i].y;
              ++count;
            }
          }
        }
      }
    }
    if (count > 0) {
      center.x /= count;
      center.y /= count;

      steer.x = center.x - _particles._position[index].x;
      steer.y = center.y - _particles._position[index].y;

      double steerLength = std::sqrt(steer.x * steer.x + steer.y * steer.y);
      if (steerLength > 0) {
        steer.x /= steerLength;
        steer.y /= steerLength;
      }
    }
    return steer;
  }

  static float compute_dist(cl_float2 a, cl_float2 b) {
    cl_float2 diff{a.x - b.x, a.y - b.y};
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
  }

  void draw(SDL_Renderer *renderer) {
    _grid.draw(renderer);
    _particles.draw(renderer);
  }

private:
  void reflection(size_t index) const {
    if ((_particles._position[index].x <= _space.left && _particles._velocity[index].x < 0) ||
        (_particles._position[index].x >= _space.right && _particles._velocity[index].x > 0)) {
      _particles._velocity[index].x *= -1;
    }
    if ((_particles._position[index].y <= _space.top && _particles._velocity[index].y < 0) ||
        (_particles._position[index].y >= _space.bottom && _particles._velocity[index].y > 0)) {
      _particles._velocity[index].y *= -1;
    }
  }

  void toroid(size_t index) const {
    if (_particles._position[index].x <= _space.left && _particles._velocity[index].x < 0) {
      _particles._position[index].x = _space.right;
    } else if (_particles._position[index].x >= _space.right && _particles._velocity[index].x > 0) {
      _particles._position[index].x = _space.left;
    }
    if (_particles._position[index].y <= _space.top && _particles._velocity[index].y < 0) {
      _particles._position[index].y = _space.bottom;
    } else if (_particles._position[index].y >= _space.bottom && _particles._velocity[index].y > 0) {
      _particles._position[index].y = _space.top;
    }
  }

  Particles<cl_float2> _particles;

  float _separation_radius{10};
  float _alignment_radius{30};
  float _cohesion_radius{30};
  float _max_speed{10};

  Space _space{};
  Border _border{Border::REFLECTIVE};

  Grid<cl_float2> _grid;

  BS::thread_pool _thread_pool;
};