
#include "mutex.h"
#include <cassert>
#include <thread>
#include <vector>

int main() {
  using namespace shyam;

  {
    Mutex m;
    int counter = 0;
    constexpr int kThreads = 8;
    constexpr int kIters = 100000;

    std::vector<std::thread> threads;
    threads.reserve(kThreads);

    for (int t = 0; t < kThreads; ++t) {
      threads.emplace_back([&] {
        for (int i = 0; i < kIters; ++i) {
          m.Lock();
          ++counter;
          m.Unlock();
        }
      });
    }

    for (auto &th : threads) {
      th.join();
    }

    assert(counter == kThreads * kIters);
  }
}
