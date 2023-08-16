//
// Created by leon- on 12/08/2023.
//

#pragma once

#include <chrono>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include <particle/particle.h>

template <Dimension S> class Grid {};

template <> class Grid<Space2D> {
  template <Dimension> friend class BoidsSimulation;

public:
  Grid(size_t width, size_t height, size_t grid_size)
      : _x_size(width / grid_size), _y_size(height / grid_size),
        _grid_size(grid_size), _grid(_x_size) {
    for (auto &row : _grid) {
      row.resize(_y_size);
    }
  }

  void draw(SDL_Renderer *renderer) const {
    int x = 0;
    int y = 0;
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 10);
    for (int i = 0; i <= _x_size; ++i) {
      SDL_RenderDrawLine(renderer, x, 0, x, _grid_size * _y_size);
      x += _grid_size;
    }
    for (int i = 0; i <= _y_size; ++i) {
      SDL_RenderDrawLine(renderer, 0, y, _grid_size * _x_size, y);
      y += _grid_size;
    }
  }

  void update(const Particles<Space2D> &particles) {
    for (auto &row : _grid) {
      for (auto &col : row) {
        col.clear();
      }
    }
    for (size_t i = 0; i < particles.size(); ++i) {
      size_t cell_x = particles._position[i].x / _grid_size;
      size_t cell_y = particles._position[i].y / _grid_size;
      if (cell_x >= _grid.size()) {
        cell_x = _grid.size() - 1;
      }
      if (cell_y >= _grid[cell_x].size()) {
        cell_y = _grid[cell_x].size() - 1;
      }
      _grid[cell_x][cell_y].push_back(i);
    }
  }

  [[nodiscard]] std::set<size_t> get_at_border() const {
    std::set<size_t> res;
    // top
    for (const auto &i : _grid.front()) {
      res.insert(i.begin(), i.end());
    }
    // bottom
    for (const auto &i : _grid.back()) {
      res.insert(i.begin(), i.end());
    }
    // sides
    for (auto &row : _grid) {
      // left
      res.insert(row.front().begin(), row.front().end());
      // right
      res.insert(row.back().begin(), row.back().end());
    }
    return res;
  }

  [[nodiscard]] size_t grid_size() const { return _grid_size; }

  [[nodiscard]] size_t x_size() const { return _x_size; }
  [[nodiscard]] size_t y_size() const { return _y_size; }

  std::vector<std::vector<size_t>> *data() { return _grid.data(); }

private:
  size_t _grid_size;
  size_t _x_size;
  size_t _y_size;
  std::vector<std::vector<std::vector<size_t>>> _grid;
};