//
// Created by leon- on 10/08/2023.
//

#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <random>
#include "Framework.h"
#include <chrono>
#include <iostream>

#define WIDTH 1200
#define HEIGHT 800

int main(int argc, char **argv) {
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);

    BoidsSimulation simulation;

    std::random_device rd;  // Used to seed the engine
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_int_distribution<int> get_w(50, 100); // Range from 1 to 100
    std::uniform_int_distribution<int> get_h(50, 100); // Range from 1 to 100

    for (int i = 0; i < 700; ++i) {
        simulation.add_boid(Ball({get_w(gen), get_h(gen)}, 1, {0.5, 0.5}));
    }

    // Creating the object by passing Height and Width value.
    Framework fw(simulation, HEIGHT, WIDTH);

    SDL_Event event{};
    unsigned FPS;
    while (event.type != SDL_QUIT) {
        auto start = std::chrono::high_resolution_clock::now();
        SDL_Delay(10);
        SDL_PollEvent(&event);
        fw.update();
        fw.draw();
        auto end = std::chrono::high_resolution_clock::now();
        FPS = 1000 / std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << FPS << std::endl;
    }

    return 1;
}
