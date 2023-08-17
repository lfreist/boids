#pragma once

#include <SDL.h>
#include <cmath>
#include <vector>
#include <memory>

#include <particle/boids.h>

template <Dimension S>
class Framework {
public:
    Framework(BoidsSimulation<S>* sim, int height, int width) : _height(height), _width(width), _sim(sim) {
        SDL_Init(SDL_INIT_VIDEO);       // Initializing SDL as Video
        SDL_CreateWindowAndRenderer(_width, _height, 0, &_window, &_renderer);
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);      // setting draw color
        SDL_RenderClear(_renderer);      // Clear the newly created window
        SDL_RenderPresent(_renderer);    // Reflects the changes done in the
        _sim->_particles.set_random_positions(10, width- 10, 10, _height - 10);
    }

    // Destructor
    ~Framework() {
        SDL_DestroyRenderer(_renderer);
        SDL_DestroyWindow(_window);
        SDL_Quit();
    }

    void update(Duration duration) {
        _sim->update(duration);
    }

    void draw() {
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
        SDL_RenderClear(_renderer);

        _sim->draw(_renderer);

        SDL_RenderPresent(_renderer);
    }

private:
    int _height;     // Height of the window
    int _width;      // Width of the window
    SDL_Renderer *_renderer = nullptr;      // Pointer for the renderer
    SDL_Window *_window = nullptr;          // Pointer for the window
    BoidsSimulation<Space2D>* _sim;
    float _gravity {0.1};
};
