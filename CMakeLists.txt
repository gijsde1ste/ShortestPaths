# Usage: git clone git://github.com/thomasmoelhave/tpie && mkdir build && cd build && cmake .. && make main && ./main

include_directories(include)

add_subdirectory(tpie)
add_executable(main main.cpp)
target_link_libraries(main tpie)

