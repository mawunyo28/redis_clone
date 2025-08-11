#ifndef R_UTIL_H
#define R_UTIL_H

#include <cstdlib>
#include <iostream>

inline void die(const std::string &msg) {
  std::cerr << "Fatal Error: " << msg << std::endl;
  exit(EXIT_FAILURE);
}

inline void msg(const std::string &msg) {
  std::cerr << "Critical error: " << msg << std::endl;
}

#endif
