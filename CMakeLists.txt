cmake_minimum_required(VERSION 3.16)
find_package(Catch2 3 REQUIRED)

cmake_minimum_required(VERSION 3.16)
project(ooplab3)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fprofile-arcs -ftest-coverage")
#-fprofile-arcs -ftest-coverage
#gcovr -r .. -e CMakeFiles --html-details -o coverage.html
add_subdirectory(dungeon)
add_subdirectory(creature)

target_link_libraries(dungeon character)
target_link_libraries(golem cell)

add_executable(main lab3main.cpp)
target_link_libraries(main dungeon)

add_executable(testing testing.cpp)
target_link_libraries(testing PRIVATE Catch2::Catch2WithMain dungeon)