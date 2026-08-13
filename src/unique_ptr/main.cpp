#include <cassert>
#include <iostream>

#include "unique_ptr.h"

int main() {
  using shyam::unique_ptr;

  // Default constructor – owns nothing
  {
    unique_ptr<int> p;
    assert(!p.is_owning());
    assert(!p);
  }

  // Constructor from raw pointer
  {
    unique_ptr<int> p(new int(42));
    assert(p.is_owning());
    assert(p);
    assert(*p == 42);
  }

  // Move constructor
  {
    unique_ptr<int> a(new int(10));
    unique_ptr<int> b = std::move(a);

    assert(!a.is_owning()); // source must be empty
    assert(b.is_owning());
    assert(*b == 10);
  }

  // Move assignment
  {
    unique_ptr<int> a(new int(20));
    unique_ptr<int> b(new int(30));

    b = std::move(a);

    assert(!a.is_owning());
    assert(b.is_owning());
    assert(*b == 20); // old value of b was deleted
  }

  // release()

  {
    unique_ptr<int> p(new int(50));
    int *raw = p.release();

    assert(!p.is_owning());
    assert(raw != nullptr);
    assert(*raw == 50);
    delete raw; // we are now responsible
  }

  // reset()
  {
    unique_ptr<int> p(new int(60));
    p.reset(new int(70));
    assert(p.is_owning());
    assert(*p == 70);

    p.reset(); // reset to nullptr
    assert(!p.is_owning());
  }

  // operator* and operator->
  {
    struct Point {
      int x, y;
    };
    unique_ptr<Point> p(new Point{3, 4});

    assert((*p).x == 3);
    assert(p->y == 4);
  }

  // Copy operations are deleted / not usable
  {
    //  The following lines must not compile.
    //  Uncommenting them should give a compiler error.

    unique_ptr<int> a(new int(1));
    //
    // copy constructor
    // unique_ptr<int> b = a;
    //
    //  copy assignment
    // b = a;
  }

  std::cout << "All assertions passed\n";
  return 0;
}
