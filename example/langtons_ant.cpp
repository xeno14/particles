#include "particles/particles.hpp"

#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>

typedef particles::Vec<int, 2> vec_t;

struct Ant {
  vec_t pos;    // position
  vec_t dir;    // direction

  Ant(int x, int y) : pos({x, y}), dir({1, 0}) {}
  void turn_left()  { dir = {-dir[1],  dir[0]}; }
  void turn_right() { dir = { dir[1], -dir[0]}; }
  void move() { pos += dir; }
};


struct Field {
  const int width;
  const int height;
  std::vector<short> field;  // 0: white, 1: black

  Field(int w, int h) : width(w), height(h), field(w*h) {}

  // Accessor
  short& operator() (int x, int y) {
    return field[x + y*width];
  }
  // Accessor
  short& operator() (const vec_t& pos) {
    return (*this)(pos[0], pos[1]);
  }
  // Move position inside of this field
  void periodic_boundary(vec_t& pos) {
    if (pos[0] < 0) pos[0] += width;
    if (pos[0] >= width) pos[0] -= width;
    if (pos[1] < 0) pos[1] += height;
    if (pos[1] >= height) pos[1] -= height;
  }
  // Output to stdout
  void output() {
    for (int y=0; y < height; y++) {
      for (int x=0; x < width; x++) {
        std::cout << ((*this)(x, y) ? "[]" : "..");
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
};


int main() {
  const int width = 40;
  const int height = 40;
  Ant ant(width/2, height/2);
  Field field(width, height);

  // main loop
  for (int t=1; t <= 30000; t++) {
    if (field(ant.pos)) { // black
      ant.turn_left();
    } else {              // white
      ant.turn_right();
    }
    field(ant.pos) = 1 - field(ant.pos);  // flip the color
    ant.move();
    field.periodic_boundary(ant.pos);

    // output
    printf("\033[2J\033[0;0H");   // clear terminal
    std::cout << "t=" << t << " @ "  << ant.pos[0] << " " << ant.pos[1] << "\n";
    field.output();

    std::this_thread::sleep_for (std::chrono::milliseconds(50));
  }

  return 0;
}
