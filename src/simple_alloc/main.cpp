
#include "simple_alloc.h"
#include <assert.h>

int main() {
  using Alloc = shyam::SimpleAlloc;

  {
    // basic allocation
    Alloc a(128);
    std::byte *p = a.Allocate(16, 8);
    assert(p != nullptr);
    assert(reinterpret_cast<std::uintptr_t>(p) % 8 == 0);
  }

  {
    // Alignment is respected for larger alignments (size rounding, not address)
    Alloc a(256);
    std::byte *p = a.Allocate(7, 32);
    assert(p != nullptr);

    auto *hdr =
        reinterpret_cast<shyam::ChunkHeader *>(p - sizeof(shyam::ChunkHeader));
    assert(hdr->Magic == shyam::ChunkHeader::HeaderMagicId);
    assert(hdr->Size % 32 == 0);
    assert(hdr->Size >= 7);

    a.Deallocate(p);
  }

  {
    // Out-of-memory throws
    Alloc a(64);
    bool threw = false;
    try {
      a.Allocate(1000);
    } catch (const std::bad_alloc &) {
      threw = true;
    }
    assert(threw);
  }

  {
    // Normal deallocate works
    Alloc a(128);
    std::byte *p = a.Allocate(24, 8);
    assert(p != nullptr);
    a.Deallocate(p); // should not throw
  }

  {
    // Deallocate of unknown / already-freed pointer throws
    Alloc a(128);
    std::byte *p = a.Allocate(16);

    a.Deallocate(p); // first free – ok

    bool threw = false;
    try {
      a.Deallocate(p); // second free – must throw
    } catch (const std::bad_alloc &) {
      threw = true;
    }
    assert(threw);
  }

  {
    // Deallocate(nullptr) is safe
    Alloc a(64);
    a.Deallocate(nullptr); // must not throw
  }

  {
    // LIFO free actually reclaims space
    Alloc a(128);
    a.Allocate(32);
    std::byte *p2 = a.Allocate(32);

    a.Deallocate(p2);
    std::byte *p3 = a.Allocate(32);
    assert(p3 != nullptr);
  }
}
