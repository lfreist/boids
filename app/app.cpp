//
// Created by leon- on 10/08/2023.
//

#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <random>
#include <particle/Framework.h>
#include <chrono>
#include <iostream>

#define WIDTH 701
#define HEIGHT 301

int main(int argc, char **argv) {
    SDL_SetMainReady();

    BoidsSimulation simulation(3000);

    // Creating the object by passing Height and Width value.
    Framework fw(simulation, HEIGHT, WIDTH);

    SDL_Event event{};
    unsigned FPS;
    auto start = std::chrono::high_resolution_clock::now();
    while (event.type != SDL_QUIT) {
        SDL_Delay(10);
        SDL_PollEvent(&event);
        auto end = std::chrono::high_resolution_clock::now();
        fw.update();
      FPS = 1000 / std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
      std::cout << "FPS: " << FPS << "\r" << std::flush;
      start = end;
      fw.draw();
    }

    return 1;
}
