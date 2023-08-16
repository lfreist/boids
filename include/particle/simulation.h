/**
 * Copyright 2023, Leon Freist (https://github.com/lfreist)
 * Author: Leon Freist <freist.leon@gmail.com>
 *
 * This file is part of kiss-ocl.
 */

#pragma once
#include <SDL.h>

#include <particle/types.h>
#include <particle/grid.h>
#include <particle/particle.h>
#include <particle/utils/thread_pool.h>


template <Dimension S> class Simulation {
public:
  Simulation() = default;
  Simulation(size_t num_particles, Grid<S> grid, uint num_threads = 0)
      : _particles(num_particles), _grid(grid),
        _thread_pool(num_worker_threads(num_threads)) {}

  virtual void draw(SDL_Renderer *renderer) = 0;
  virtual void update(Duration elapsed) = 0;

protected:
  static uint num_worker_threads(uint nt) {
    nt = nt > 0 ? nt : 1;
    return nt < std::thread::hardware_concurrency()
               ? nt
               : std::thread::hardware_concurrency();
  }

  Particles<S> _particles{};
  Space _space{0, 0, 100, 100};
  BORDER _border{BORDER::REFLECTIVE};

  Grid<S> _grid{};

  BS::thread_pool _thread_pool{std::thread::hardware_concurrency()};
};