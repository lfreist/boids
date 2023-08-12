//
// Created by leon- on 12/08/2023.
//

#pragma once

#include <vector>
#include <map>
#include <set>
#include "Ball.h"


class Grid {
public:
    Grid(size_t width, size_t height, size_t grid_size) : _x_size(width / grid_size), _y_size(height / grid_size), _grid_size(grid_size) {
        _grid.resize(_x_size, std::vector<std::vector<Ball*>>(_y_size));
    }

    void draw(SDL_Renderer* renderer) {
        int x = 0;
        int y = 0;
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 100);
        for (int i = 0; i <= _x_size; ++i) {
            SDL_RenderDrawLine(renderer, x, 0, x, _grid_size * _y_size);
            x += _grid_size;
        }
        for (int i = 0; i <= _y_size; ++i) {
            SDL_RenderDrawLine(renderer, 0, y, _grid_size * _x_size, y);
            y += _grid_size;
        }
    }

    void update(std::vector<Ball>& particles) {
        for (auto& row : _grid) {
            for (auto& col : row) {
                col.clear();
            }
        }
        for (auto& p : particles) {
            size_t cell_x = p._position.x / _grid_size;
            size_t cell_y = p._position.y / _grid_size;
            if (cell_x >= _grid.size()) {
                cell_x = _grid.size() -1;
            }
            if (cell_y >= _grid[cell_x].size()) {
                cell_y = _grid[cell_x].size() -1;
            }
            _grid[cell_x][cell_y].push_back(&p);
        }
    }

    [[nodiscard]] std::set<Ball*> get_at_border() const {
        std::set<Ball*> res;
        // top
        for (const auto & i : _grid.front()) {
            res.insert(i.begin(), i.end());
        }
        // bottom
        for (const auto & i : _grid.back()) {
            res.insert(i.begin(), i.end());
        }
        // sides
        for (auto& row : _grid) {
            // left
            res.insert(row.front().begin(), row.front().end());
            // right
            res.insert(row.back().begin(), row.back().end());
        }
        return res;
    }

    [[nodiscard]] std::set<Ball*> get_neighbors(const Ball& boid) const {
        std::set<Ball*> neighbors;
        int x = boid._position.x / _grid_size;
        int y = boid._position.y / _grid_size;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                int n_x = x + dx;
                int n_y = y + dy;
                if (n_x >= 0 && n_x < _x_size && n_y >= 0 && n_y < _y_size) {
                    neighbors.insert(_grid[n_x][n_y].begin(), _grid[n_x][n_y].end());
                }
            }
        }
        return neighbors;
    }

private:
    size_t _grid_size;
    size_t _x_size;
    size_t _y_size;
    std::vector<std::vector<std::vector<Ball*>>> _grid;
};