#pragma once

#include <SDL.h>
#include <cmath>
#include <vector>
#include <memory>

#include "boids.h"

class Framework {
    friend class Ball;
public:
    // Contructor which initialize the parameters.
    Framework(BoidsSimulation& sim, int height_, int width_) : _height(height_), _width(width_), _sim(&sim) {
        SDL_Init(SDL_INIT_VIDEO);       // Initializing SDL as Video
        SDL_CreateWindowAndRenderer(_width, _height, 0, &_window, &_renderer);
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);      // setting draw color
        SDL_RenderClear(_renderer);      // Clear the newly created window
        SDL_RenderPresent(_renderer);    // Reflects the changes done in the
        //  window.
    }

    // Destructor
    ~Framework() {
        SDL_DestroyRenderer(_renderer);
        SDL_DestroyWindow(_window);
        SDL_Quit();
    }

    void update() {
        _sim->updateBoids();
    }

    void draw() {
        // Setting the color to be RED with 100% opaque (0% trasparent).
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
        SDL_RenderClear(_renderer);

        _sim->draw(_renderer);

        // Show the change on the screen
        SDL_RenderPresent(_renderer);
    }

private:
    int _height;     // Height of the window
    int _width;      // Width of the window
    SDL_Renderer *_renderer = nullptr;      // Pointer for the renderer
    SDL_Window *_window = nullptr;          // Pointer for the window
    BoidsSimulation* _sim;
    float _gravity {0.1};
};
