

#include "lock_free_ring_buffer.h"

#include <cassert>
#include <cstdio>
#include <string>
#include <utility>

namespace {

struct Tracer {
  int id = 0;
  std::string name;

  Tracer() = default;
  Tracer(int i, std::string n) : id(i), name(std::move(n)) {}

  Tracer(const Tracer &other) : id(other.id), name(other.name) {
    std::puts("copy ctor");
  }
  Tracer &operator=(const Tracer &other) {
    std::puts("copy assign");
    id = other.id;
    name = other.name;
    return *this;
  }
  Tracer(Tracer &&other) noexcept : id(other.id), name(std::move(other.name)) {
    other.id = -1;
  }
  Tracer &operator=(Tracer &&other) noexcept {
    id = other.id;
    name = std::move(other.name);
    other.id = -1;
    return *this;
  }
  ~Tracer() = default;
};

void test_empty_pop() {
  shyam::RingBuffer<int, 4> q;
  int out = 42;
  assert(!q.TryPop(out));
  assert(out == 42); // unchanged on failure
}

void test_single_push_pop_int() {
  shyam::RingBuffer<int, 4> q;
  assert(q.TryPush(7));
  int out = 0;
  assert(q.TryPop(out));
  assert(out == 7);
  assert(!q.TryPop(out));
}

void test_fill_then_full() {
  // Cap = 4 → usable slots = 3
  shyam::RingBuffer<int, 4> q;
  assert(q.TryPush(1));
  assert(q.TryPush(2));
  assert(q.TryPush(3));
  assert(!q.TryPush(4)); // full

  int out = 0;
  assert(q.TryPop(out) && out == 1);
  assert(q.TryPush(4)); // space again
  assert(q.TryPop(out) && out == 2);
  assert(q.TryPop(out) && out == 3);
  assert(q.TryPop(out) && out == 4);
  assert(!q.TryPop(out));
}

void test_wraparound() {
  shyam::RingBuffer<int, 4> q;
  int out = 0;

  // Drive indices around the ring several times
  for (int cycle = 0; cycle < 5; ++cycle) {
    assert(q.TryPush(10 + cycle));
    assert(q.TryPush(20 + cycle));
    assert(q.TryPop(out) && out == 10 + cycle);
    assert(q.TryPush(30 + cycle));
    assert(q.TryPop(out) && out == 20 + cycle);
    assert(q.TryPop(out) && out == 30 + cycle);
    assert(!q.TryPop(out));
  }
}

void test_fifo_order() {
  shyam::RingBuffer<int, 8> q;
  for (int i = 0; i < 7; ++i) {
    assert(q.TryPush(100 + i));
  }
  assert(!q.TryPush(999));

  int out = 0;
  for (int i = 0; i < 7; ++i) {
    assert(q.TryPop(out));
    assert(out == 100 + i);
  }
  assert(!q.TryPop(out));
}

void test_user_type_move() {
  shyam::RingBuffer<Tracer, 4> q;

  assert(q.TryPush(Tracer{1, "a"}));
  assert(q.TryPush(Tracer{2, "b"}));
  assert(q.TryPush(Tracer{3, "c"}));
  assert(!q.TryPush(Tracer{4, "d"}));

  Tracer out;
  assert(q.TryPop(out));
  assert(out.id == 1 && out.name == "a");

  assert(q.TryPop(out));
  assert(out.id == 2 && out.name == "b");

  assert(q.TryPush(Tracer{4, "d"}));

  assert(q.TryPop(out));
  assert(out.id == 3 && out.name == "c");

  assert(q.TryPop(out));
  assert(out.id == 4 && out.name == "d");

  assert(!q.TryPop(out));
}

void test_string() {
  shyam::RingBuffer<std::string, 4> q;
  assert(q.TryPush(std::string("hello")));
  assert(q.TryPush(std::string("world")));

  std::string out;
  assert(q.TryPop(out) && out == "hello");
  assert(q.TryPop(out) && out == "world");
  assert(!q.TryPop(out));
}

} // namespace

int main() {
  test_empty_pop();
  test_single_push_pop_int();
  test_fill_then_full();
  test_wraparound();
  test_fifo_order();
  test_user_type_move();
  test_string();

  std::puts("all ring buffer tests passed");
  return 0;
}
