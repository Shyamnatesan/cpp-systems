

#include <atomic>
#include <thread>

namespace shyam {

class Mutex {
public:
  Mutex() = default;

  Mutex(const Mutex &) = delete;

  void operator=(const Mutex &) = delete;

  void Lock() {
    // here, we want to hold the lock.
    // meaning we want to set true to the lock.
    // but only when it is false. if it is true, the lock is already beinig held
    // by someone else. so we spin

    for (;;) {
      auto expected{false};

      // we try to acquire the lock 10 times.
      for (int spin{}; spin < 10; ++spin) {
        // try to acquire
        if (flag_.compare_exchange_weak(expected, true,
                                        std::memory_order::acquire,
                                        std::memory_order::relaxed)) {
          // if success, return immediately.
          return;
        }
        // upon failure, set this to false
        expected = false;
      }
      // after 10 failed attempts, yield this thread.
      std::this_thread::yield();
    }
  }

  void Unlock() {
    // here, we want to release the lock.
    // meaning, we want to set the lock back to false.
    // assuming the caller holds the lock.
    flag_.store(false, std::memory_order::release);
  }

private:
  // atomic<bool> is enough here to indicate whether the lock is held or not.
  // So, 2 states true or false is enough.
  // true -> lock is being held.
  // false -> lock is not being held.
  std::atomic_bool flag_{false};
};

} // namespace shyam
