#pragma once

struct Position {
    int x;
    int y;
};

struct Velocity {
    float x;
    float y;
};

class Ball {
    friend class BoidsSimulation;
public:
    Ball(Position position, int radius, Velocity velocity = {0.0, 0.0}) :
            _position(position),
            _radius(radius),
            _velocity(velocity) {}

    void update_color(float max_speed) {
        _color = {static_cast<Uint8>(_velocity.x / max_speed * 255), 0, static_cast<Uint8>(_velocity.y / max_speed * 255), 155};
    }

    void draw(SDL_Renderer *renderer) const {
        SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);

        // Drawing circle
        for (int x = _position.x - _radius; x <= _position.x + _radius; x++) {
            for (int y = _position.y - _radius; y <= _position.y + _radius; y++) {
                if ((std::pow(_position.y - y, 2) + std::pow(_position.x - x, 2)) <=
                    std::pow(_radius, 2)) {
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }
    }

private:
    Position _position;
    Velocity _velocity;
    SDL_Color _color {255, 0, 0, 255};
    int _radius;
};
