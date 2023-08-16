//
// Created by leon- on 11/08/2023.
//

#pragma once

#include <cmath>
#include <vector>

#include <particle/grid.h>
#include <particle/particle.h>
#include <particle/simulation.h>
#include <particle/types.h>

template <Dimension S>
class BoidsSimulation : public Simulation<S> {};

template <>
class BoidsSimulation<Space2D> : public Simulation<Space2D> {
  template <Dimension> friend class Framework;
public:
  explicit BoidsSimulation(size_t num_particles, Grid<Space2D> grid, uint num_threads) : Simulation<Space2D>(num_particles, grid, num_threads) {}

  void update(Duration duration) override {
    _grid.update(_particles);
    updateBoids();
  }
  
  void draw(SDL_Renderer *renderer) override {
    // _grid.draw(renderer);
    _particles.draw(renderer);
  }

private:
  void updateBoids() {
    auto apply_boids_rules = [this](int beg, int end) {
      for (size_t i = beg; i < end; ++i) {
        cl_float2 sep = separate(i);
        cl_float2 ali = align(i);
        cl_float2 coh = cohere(i);
        
        _particles.velocity_data()[i].x += sep.x + ali.x + coh.x;
        _particles.velocity_data()[i].y += sep.y + ali.y + coh.y;

        double speed =
            std::sqrt(_particles.velocity_data()[i].x * _particles.velocity_data()[i].x +
                      _particles.velocity_data()[i].y * _particles.velocity_data()[i].y);

        if (speed > _max_speed) {
          _particles.velocity_data()[i].x = (_particles.velocity_data()[i].x / speed) * _max_speed;
          _particles.velocity_data()[i].y = (_particles.velocity_data()[i].y / speed) * _max_speed;
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
        case BORDER::REFLECTIVE:
          reflection(bb[i]);
          break;
        case BORDER::TOROIDAL:
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
        _particles.position_data()[i].x += _particles.velocity_data()[i].x;
        _particles.position_data()[i].y += _particles.velocity_data()[i].y;
        _particles.color_data()[i].x = (_particles.velocity_data()[i].x / _max_speed) * 255;
        _particles.color_data()[i].y = (_particles.velocity_data()[i].y / _max_speed) * 255;
        _particles.color_data()[i].w = 255;
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
          for (size_t i : _grid.data()[n_x][n_y]) {
            double distance = compute_dist(_particles.position_data()[index],
                                           _particles.position_data()[i]);
            if (distance > 0 && distance < _separation_radius) {
              // Calculate the vector pointing away from other boids
              double diffX = _particles.position_data()[index].x - _particles.position_data()[i].x;
              double diffY = _particles.position_data()[index].y - _particles.position_data()[i].y;
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
    int x = _particles.position_data()[index].x / _grid.grid_size();
    int y = _particles.position_data()[index].y / _grid.grid_size();
    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        int n_x = x + dx;
        int n_y = y + dy;
        if (n_x >= 0 && n_x < _grid.x_size() && n_y >= 0 &&
            n_y < _grid.y_size()) {
          for (size_t i : _grid.data()[n_x][n_y]) {
            double distance = compute_dist(_particles.position_data()[index], _particles.position_data()[i]);
            if (distance > 0 && distance < _alignment_radius) {
              average.x += _particles.velocity_data()[i].x;
              average.y += _particles.velocity_data()[i].y;
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

    int x = _particles.position_data()[index].x / _grid.grid_size();
    int y = _particles.position_data()[index].y / _grid.grid_size();
    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        int n_x = x + dx;
        int n_y = y + dy;
        if (n_x >= 0 && n_x < _grid.x_size() && n_y >= 0 &&
            n_y < _grid.y_size()) {
          for (size_t i : _grid.data()[n_x][n_y]) {
            double distance = compute_dist(_particles.position_data()[index], _particles.position_data()[i]);
            if (distance > 0 && distance < _cohesion_radius) {
              center.x += _particles.position_data()[i].x;
              center.y += _particles.position_data()[i].y;
              ++count;
            }
          }
        }
      }
    }
    if (count > 0) {
      center.x /= count;
      center.y /= count;

      steer.x = center.x - _particles.position_data()[index].x;
      steer.y = center.y - _particles.position_data()[index].y;

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

  
  void reflection(size_t index) {
    if ((_particles.position_data()[index].x <= _space.position.x && _particles.velocity_data()[index].x < 0) ||
        (_particles.position_data()[index].x >= _space.position.x + _space.width() && _particles.velocity_data()[index].x > 0)) {
      _particles.velocity_data()[index].x *= -1;
    }
    if ((_particles.position_data()[index].y <= _space.position.y && _particles.velocity_data()[index].y < 0) ||
        (_particles.position_data()[index].y >= _space.position.y + _space.height() && _particles.velocity_data()[index].y > 0)) {
      _particles.velocity_data()[index].y *= -1;
    }
  }

  void toroid(size_t index) {
    if (_particles.position_data()[index].x <= _space.position.x && _particles.velocity_data()[index].x < 0) {
      _particles.position_data()[index].x = _space.position.x;
    } else if (_particles.position_data()[index].x >= _space.position.x + _space.width() && _particles.velocity_data()[index].x > 0) {
      _particles.position_data()[index].x = _space.position.x + _space.width();
    }
    if (_particles.position_data()[index].y <= _space.position.y && _particles.velocity_data()[index].y < 0) {
      _particles.position_data()[index].y = _space.position.y + _space.height();
    } else if (_particles.position_data()[index].y >= _space.position.y + _space.height() && _particles.velocity_data()[index].y > 0) {
      _particles.position_data()[index].y = _space.position.y;
    }
  }

  float _separation_radius{10};
  float _alignment_radius{30};
  float _cohesion_radius{30};
  float _max_speed{7};
};