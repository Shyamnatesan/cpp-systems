

#include <cassert>
#include <iostream>

#include "mem_pool.h"

int main() {
  using shyam::MemPool;
  using shyam::Message;

  //  Construction – everything starts free
  {
    MemPool pool(4);
    assert(pool.available() == 4);
  }

  // Basic allocate + get
  {
    MemPool pool(3);
    Message m{42, 100};
    int h = pool.allocate(m);
    assert(h == 0); // first free slot is 0
    assert(pool.available() == 2);

    Message *p = pool.get(h);
    assert(p != nullptr);
    assert(p->id == 42);
    assert(p->size == 100);
  }

  // Exhaust the pool
  {
    MemPool pool(2);
    assert(pool.allocate({1, 10}) == 0);
    assert(pool.allocate({2, 20}) == 1);
    assert(pool.allocate({3, 30}) == -1); // full
    assert(pool.available() == 0);
  }

  // release + LIFO behaviour
  {
    MemPool pool(3);
    pool.allocate({10, 1});
    int h1 = pool.allocate({20, 2});
    pool.allocate({30, 3});
    assert(pool.available() == 0);

    assert(pool.release(h1) == true);
    assert(pool.available() == 1);

    // Most recently released slot should be handed out next
    int h = pool.allocate({99, 9});
    assert(h == h1);
    assert(pool.get(h)->id == 99);
  }

  // Invalid / double release
  {
    MemPool pool(2);
    int h = pool.allocate({1, 1});

    assert(pool.release(h) == true);
    assert(pool.release(h) == false); // already free
    assert(pool.release(-1) == false);
    assert(pool.release(100) == false);
    assert(pool.release(2) == false); // == capacity
  }

  // get on free / invalid handles
  {
    MemPool pool(2);
    int h = pool.allocate({5, 5});
    pool.release(h);

    assert(pool.get(h) == nullptr); // now free
    assert(pool.get(-1) == nullptr);
    assert(pool.get(99) == nullptr);
  }

  // reset restores original free-list order
  {
    MemPool pool(3);
    pool.allocate({1, 1});
    pool.allocate({2, 2});
    pool.release(0); // free list is no longer 0→1→2

    pool.reset();
    assert(pool.available() == 3);

    // After reset the free list must start at 0 again
    int h = pool.allocate({7, 7});
    assert(h == 0);
  }

  std::cout << "All assertions passed\n";
  return 0;
}
