add_executable(app app.cpp)
target_link_libraries(app PRIVATE SDL2::SDL2 opencl)