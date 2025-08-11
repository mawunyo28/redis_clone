#pragma once
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

static int32_t read_full(int fd, char *buf, size_t n) {
  while (n > 0) {
    ssize_t rv = read(fd, buf, n);

    if (rv <= 0) {
      return -1; // error, or unexpected EOF
    }

    assert((size_t)rv <= n);
    n -= (size_t)rv;

    buf += rv;
  }

  return 0;
}

static int32_t write_all(int fd, const char *buf, size_t n) {
  while (n > 0) {
    ssize_t rv = write(fd, buf, n);

    if (rv <= 0) {
      return -1; // error
    }

    assert((size_t)rv <= n);

    n -= (size_t)rv;

    buf += rv;
  }
  return 0;
}

const size_t k_max_msg = 4096;

inline void die(const std::string &msg) {
  std::cerr << "Fatal Error: " << msg << std::endl;
  exit(EXIT_FAILURE);
}

inline void msg(const std::string &msg) {
  std::cerr << "Critical error: " << msg << std::endl;
}
