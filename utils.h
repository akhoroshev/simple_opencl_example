#pragma once

#include <chrono>
#include <fstream>
#include <tuple>

struct timer {
  using ms = std::chrono::milliseconds;

  timer() { start(); }

  void start() { t1 = std::chrono::high_resolution_clock::now(); }

  template <typename Duration> size_t stop() {
    auto t2 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<Duration>(t2 - t1).count();
  }

private:
  std::chrono::time_point<std::chrono::system_clock> t1;
};

inline float calc_bandwidth_gb_s(size_t sz_in_bytes, float time_in_ms) {
  return sz_in_bytes / 1024 / 1024 / 1024 / (time_in_ms / 1000);
}

inline std::string loadProgram(const char *filename) {
  std::ifstream file(filename);
  return {std::istreambuf_iterator<char>(file),
          std::istreambuf_iterator<char>()};
}

template <typename T> inline auto make_data(size_t size) {
  std::vector<T> a(size);
  std::vector<T> b(size);
  std::vector<T> c(size);

  // Fill bit array
  for (size_t i = 0; i < size; i++) {
    a[i] = i * i;
    b[i] = i + 42;
  }

  return std::make_tuple(std::move(a), std::move(b), std::move(c));
}