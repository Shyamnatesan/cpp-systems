# cpp-systems

Small, focused implementations of core systems programming concepts in modern C++.

This repository is a personal learning / reference collection. Each program explores the internal mechanics of a fundamental building block (containers, allocators, memory management, synchronization, and compile-time computation). The goal is understanding, not production use.

## Layout

- `include/shyam/` — header-only components
- `examples/` — one driver per component
- `simpledb/` — separate program, not part of this build

## Namespaces

| Namespace | Components |
| --- | --- |
| `shyam::containers` | `Vector` |
| `shyam::memory` | `Arena`, `MallocStack`, `MemPool`, `SimpleAlloc`, `unique_ptr` |
| `shyam::sync` | `Mutex`, `RingBuffer` |
| `shyam::meta` | `Fibonacci`, `Numbers`, `Accumulate`, `ValueAt` |
| `shyam::perf` | L1 cold-read measurement (example only) |

`Numbers` lives once, in `shyam::meta`, and is shared by the sum and index-access templates so the two headers can be included together.

## Build

CMake 3.24 or newer, and a C++23 compiler.

```bash
cmake -S . -B build
cmake --build build
./build/shyam_vector
```

AddressSanitizer and UndefinedBehaviorSanitizer are on by default. Turn them off with `-DENABLE_SANITIZERS=OFF`.

## Programs

- `shyam_vector`
- `shyam_malloc_stack`
- `shyam_simple_alloc`
- `shyam_mem_pool`
- `shyam_unique_ptr`
- `shyam_arena`
- `shyam_mutex`
- `shyam_lock_free_ring_buffer`
- `shyam_l1_cold_read_latency`
- `shyam_template_metaprogramming`
