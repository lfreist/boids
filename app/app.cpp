//
// Created by leon- on 10/08/2023.
//

#define SDL_MAIN_HANDLED

#include <SDL.h>

#include <chrono>
#include <iostream>
#include <random>

#include <particle/Framework.h>

#define WIDTH 1501
#define HEIGHT 901

int main(int argc, char **argv) {
  SDL_SetMainReady();

  BoidsSimulation<Space2D> simulation(3000, {WIDTH - 1, HEIGHT - 1, 15}, 16);

  // Creating the object by passing Height and Width value.
  Framework fw(&simulation, HEIGHT, WIDTH);

  SDL_Event event{};
  unsigned FPS;
  unsigned lowest = std::numeric_limits<unsigned>::max();
  unsigned highest = 0;
  unsigned sum = 0;
  unsigned count = 0;
  while (event.type != SDL_QUIT) {
    auto start = std::chrono::high_resolution_clock::now();
    SDL_Delay(10);
    SDL_PollEvent(&event);
    fw.update();
    fw.draw();
    auto end = std::chrono::high_resolution_clock::now();
    FPS = 1000 /
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
              .count();
    if (lowest > FPS) {
      lowest = FPS;
    }
    if (highest < FPS) {
      highest = FPS;
    }
    count++;
    sum += FPS;
    std::cout << "                                                          \r";
    std::cout << " FPS: " << FPS << " (max: " << highest << ", min: " << lowest
              << ", avg: " << sum / count << ")\r" << std::flush;
  }

  return 1;
}
