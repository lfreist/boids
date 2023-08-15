add_library(boids_cl boids_cl.cpp)
target_link_libraries(boids_cl PUBLIC opencl)