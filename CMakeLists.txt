# Usage: git clone git://github.com/thomasmoelhave/tpie && mkdir build && cd build && cmake .. && make main && ./main

include_directories(include)

add_subdirectory(tpie)
find_package(SDL2)

file(GLOB SOURCES "src/*.cpp")

add_executable(main ${SOURCES})
target_link_libraries(main tpie SDL2)

