//
// Created by leon- on 10/08/2023.
//

#define SDL_MAIN_HANDLED

#include <SDL.h>

#include <chrono>
#include <iostream>
#include <random>
#include <iomanip>

#include <particle/Framework.h>

#define WIDTH 1001
#define HEIGHT 401

void print_fps(int fps) {
  static int sum = 0;
  static int count = 0;
  static int min = std::numeric_limits<int>::max();
  static int max = 0;
  min = std::min(min, fps);
  max = std::max(max, fps);
  count++;
  sum += fps;
  std::cout << "                                                          \r";
  std::cout << " FPS: " << std::left << std::setw(8) << fps << " (max: " << max << ", min: " << min
            << ", avg: " << sum / count << ")\r" << std::flush;
}

int main(int argc, char **argv) {
  SDL_SetMainReady();

  BoidsSimulation<Space2D> simulation(100, {WIDTH - 1, HEIGHT - 1, 15}, 16);

  // Creating the object by passing Height and Width value.
  Framework fw(&simulation, HEIGHT, WIDTH);

  SDL_Event event{};
  unsigned FPS;
  auto start = std::chrono::high_resolution_clock::now();
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;
  bool first_loop = true;

  while (event.type != SDL_QUIT) {
    start = std::chrono::high_resolution_clock::now();
    SDL_PollEvent(&event);
    fw.update(duration);
    fw.draw();
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    if (first_loop) {
      first_loop = false;
      continue;
    }
    FPS = 1 / duration.count();
    print_fps(FPS);
  }

  return 1;
}
