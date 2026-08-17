

#include <chrono>
#include <iostream>
#include <new>
#include <vector>

namespace shyam {

double MeasureL1ColdReadLatency() {

  // get the cache line size of the underlying hardware
  constexpr size_t CACHE_LINE_SIZE =
      std::hardware_destructive_interference_size;

  // Purge buffer.
  // We make this buffer large enough that it easily exceeds a typical L1 data
  // cache.
  constexpr size_t PURGE_BUFFER_SIZE = CACHE_LINE_SIZE * (2 << 12);

  // number of iterations/epochs
  constexpr size_t ITERATIONS = 15'000;

  // the target buffer:
  //    - this sits in a single cache line
  //    - we will make this cold, and then read from it.
  // the purge buffer:
  //    - large buffer used only to thrash the cache so that 'target' is forced
  //    out of L1 before each measured read.
  std::vector<std::uint8_t> target(CACHE_LINE_SIZE, 1);
  std::vector<std::uint8_t> purge(PURGE_BUFFER_SIZE, 1);

  // volatile sink forces the compiler to actually perform every load.
  // without it, the compiler could prove the loaded values are never observed
  // and delete the memory accesses.
  volatile std::uint64_t sink = 0;

  // we will measure 2 things:
  // 1. total_time - time for the whole experiment (all evictions + all cold
  // loads)
  //
  // 2. eviction time - the portion of that time that was spent only inside the
  // eviction loops.
  //
  // the cold-load cost is then recovered by subtraction.
  // (total_time - eviction_time) / ITERATIONS
  //
  // this amortizes the timer overhead across many trials instead of paying it
  // on every single load.

  double eviction_time_ns{};

  const auto experiment_start = std::chrono::steady_clock::now();

  for (size_t iter{}; iter < ITERATIONS; ++iter) {

    // Phase 1: Force the target line out of L1
    // - we walk the eviction buffer with a stride of one cache line.
    // touching a single byte per line is enough to allocate that line in cache
    // and (with high probability) displace whatever was previously occupying
    // the same set,

    const auto eviction_start = std::chrono::steady_clock::now();

    for (size_t i{}; i < purge.size(); i += CACHE_LINE_SIZE) {
      sink += purge[i];
    }

    const auto eviction_end = std::chrono::steady_clock::now();

    eviction_time_ns += static_cast<double>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(eviction_end -
                                                             eviction_start)
            .count());

    // Phase 2: the actual cold read we care about.
    // - because we just thrashed the cache, target[0] should miss in L1 and the
    // line must be brought back in. We deliberately do only one load so that we
    // measure a single cold fill, not a mixture of hits and misses.

    sink += target[0];
  }

  const auto experiment_end = std::chrono::steady_clock::now();

  const double total_time_ns =
      static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(
                              experiment_end - experiment_start)
                              .count());

  // prevent the compiler from proving that 'sink' is unused and
  // subsequently deleting all of the loads that feed it.
  if (sink == 0) {
    // should never happen; just a compiler barrier
    return -1.0;
  }

  return (total_time_ns - eviction_time_ns) / static_cast<double>(ITERATIONS);
}
} // namespace shyam

int main() {
  std::cout << "L1 cache cold read latency: "
            << shyam::MeasureL1ColdReadLatency() << '\n';
}
