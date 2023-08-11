//
// Created by leon- on 11/08/2023.
//

#pragma once

#include <vector>
#include <cmath>

#include "Ball.h"

class BoidsSimulation {
public:
    enum class Border {
        REFLECTIVE,
        TOROIDAL,
        RESET
    };

    struct Space {
        int top = 200;
        int bottom = 600;
        int left = 200;
        int right = 1000;
    };

    BoidsSimulation() = default;

    void add_boid(Ball boid) {
        _particles.push_back(boid);
    }

    void update() {
        updateBoids();
    }

    void updateBoids() {
        for (auto& boid : _particles) {
            switch (_border) {
                case Border::REFLECTIVE:
                    reflection(boid);
                    break;
                case Border::TOROIDAL:
                    toroid(boid);
                    break;
                case Border::RESET:
                    reset(boid);
                    break;
                default:
                    reflection(boid);
                    break;
            }
            auto sep = separate(boid);
            auto ali = align(boid);
            auto coh = cohere(boid);

            Velocity acc {sep.x + ali.x + coh.x, sep.y + ali.y + coh.y};

            boid._velocity.x += acc.x;
            boid._velocity.y += acc.y;

            float speed = sqrt(boid._velocity.x * boid._velocity.x + boid._velocity.y * boid._velocity.y);
            if (speed > _max_speed) {
                boid._velocity.x = (boid._velocity.x / speed) * _max_speed;
                boid._velocity.y = (boid._velocity.y / speed) * _max_speed;
            }
            boid._position.x += static_cast<int>(boid._velocity.x);
            boid._position.y += static_cast<int>(boid._velocity.y);

            boid.update_color(_max_speed);
        }
    }

    Velocity separate(Ball& boid) {
        Velocity steer{0, 0};
        int count = 0;
        for (auto& other : _particles) {
            float distance = compute_dist(boid._position, other._position);
            if (distance > 0 && distance < _separation_radius) {
                auto dx = static_cast<float>(boid._position.x - other._position.x);
                auto dy = static_cast<float>(boid._position.y - other._position.y);
                dx /= distance;
                dy /= distance;

                steer.x += dx;
                steer.y += dy;
                count++;
            }
        }
        if (count > 0) {
            steer.x /= static_cast<float>(count);
            steer.y /= static_cast<float>(count);
            float steer_length = std::sqrt(steer.x * steer.x + steer.y * steer.y);
            if (steer_length > 0) {
                steer.x /= static_cast<float>(steer_length);
                steer.y /= static_cast<float>(steer_length);
            }
        }
        return steer;
    }

    Velocity align(const Ball& boid) {
        Velocity average{0, 0};
        int count = 0;
        for (auto& other : _particles) {
            double distance = compute_dist(boid._position, other._position);
            if (distance > 0 && distance < _alignment_radius) {
                average.x += other._velocity.x;
                average.y += other._velocity.y;
                ++count;
            }
        }
        if (count > 0) {
            average.x /= static_cast<float>(count);
            average.y /= static_cast<float>(count);
            double average_length = std::sqrt(average.x * average.x + average.y * average.y);
            if (average_length > 0) {
                average.x /= static_cast<float>(average_length);
                average.y /= static_cast<float>(average_length);
            }
        }
        return average;
    }

    Velocity cohere(const Ball& boid) {
        Velocity center{0, 0};
        Velocity steer{0, 0};
        int count = 0;

        for (auto& other : _particles) {
            double distance = compute_dist(boid._position, other._position);
            if (distance > 0 && distance < _cohesion_radius) {
                center.x += static_cast<float>(other._position.x);
                center.y += static_cast<float>(other._position.y);
                ++count;
            }
        }
        if (count > 0) {
            center.x /= static_cast<float>(count);
            center.y /= static_cast<float>(count);
            steer.x = center.x - static_cast<float>(boid._position.x);
            steer.y = center.y - static_cast<float>(boid._position.y);
            double steer_length = std::sqrt(steer.x * steer.x + steer.y * steer.y);
            if (steer_length > 0) {
                steer.x /= static_cast<float>(steer_length);
                steer.y /= static_cast<float>(steer_length);
            }
        }
        return steer;
    }

    static float compute_dist(Position a, Position b) {
        auto dx = static_cast<float>(a.x - b.x);
        auto dy = static_cast<float>(a.y - b.y);
        return std::sqrt(dx * dx + dy * dy);
    }

    void draw(SDL_Renderer* renderer) {
        for (const auto& boid : _particles) {
            boid.draw(renderer);
        }
    }

private:
    void reflection(Ball& boid) const {
        if (boid._position.x <= _space.left && boid._velocity.x < 0) {
            boid._velocity.x *= -1;
        } else if (boid._position.x >= _space.right && boid._velocity.x > 0) {
            boid._velocity.x *= -1;
        }
        if (boid._position.y <= _space.top && boid._velocity.y < 0) {
            boid._velocity.y *= -1;
        } else if (boid._position.y >= _space.bottom && boid._velocity.y > 0) {
            boid._velocity.y *= -1;
        }
    }

    void toroid(Ball& boid) const {
        if (boid._position.x <= _space.left && boid._velocity.x < 0) {
            boid._position.x = _space.right;
        } else if (boid._position.x >= _space.right && boid._velocity.x > 0) {
            boid._position.x = _space.left;
        }
        if (boid._position.y <= _space.top && boid._velocity.y < 0) {
            boid._position.y = _space.bottom;
        } else if (boid._position.y >= _space.bottom && boid._velocity.y > 0) {
            boid._position.y = _space.top;
        }
    }

    void reset(Ball& boid) const {
        if (boid._position.x <= _space.left) {
            boid._position = {(_space.right - _space.left) / 2, (_space.bottom - _space.top) / 2};;
        } else if (boid._position.x >= _space.right) {
            boid._position = {(_space.right - _space.left) / 2, (_space.bottom - _space.top) / 2};
        }
        if (boid._position.y <= _space.top) {
            boid._position = {(_space.right - _space.left) / 2, (_space.bottom - _space.top) / 2};
        } else if (boid._position.y >= _space.bottom) {
            boid._position = {(_space.right - _space.left) / 2, (_space.bottom - _space.top) / 2};
        }
    }

    std::vector<Ball> _particles;

    float _separation_radius {10};
    float _alignment_radius {30};
    float _cohesion_radius {30};
    float _max_speed {10};


    Space _space{};
    Border _border{Border::REFLECTIVE};
};