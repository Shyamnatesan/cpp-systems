

#include <algorithm>
#include <cstddef>
#include <limits>
#include <memory>
#include <new>

namespace shyam {

struct ChunkHeader {
  const static inline size_t HeaderMagicId = std::numeric_limits<size_t>::max();
  size_t Size{HeaderMagicId};
  size_t Magic{HeaderMagicId};
};

struct Chunk {
  ChunkHeader header;
  std::byte *memory{nullptr};
};

class SimpleAlloc {

public:
  // owns a freshly allocated blk
  explicit SimpleAlloc(size_t capacity)
      : capacity_(capacity), blk_(std::make_unique<std::byte[]>(capacity_)) {}

  // takes ownership of an existing blk
  SimpleAlloc(size_t capacity, std::byte *allocated)
      : capacity_(capacity), blk_(allocated) {}

  SimpleAlloc(const SimpleAlloc &) = delete;

  void operator=(const SimpleAlloc &) = delete;

  SimpleAlloc(SimpleAlloc &&) = delete;

  void operator=(const SimpleAlloc &&) = delete;

  std::byte *Allocate(size_t size, size_t align = sizeof(void *)) {

    // so we store [CHUNK1ALLOC1CHUNK2ALLOC2]
    // so, the chunk itself should be properly aligned. meaning the chunkheader
    // should be properly aligned. so, we force the alignment to be at least as
    // large as the header itself, by taking the maximum of what the caller
    // asked for and the header size.
    align = std::max(align, sizeof(ChunkHeader));

    // round the requested size UP to the next multiple of align.
    // We need the allocated block to end on an alignment boundary so that
    // the next allocation can start cleanly. The classic integer way to
    // compute the smallest multiple of `align` that is >= `size` is:
    //
    //     ((size + align - 1) / align) * align
    //
    // Example:
    //   size = 2, align = 8  →  ((2 + 8 - 1) / 8) * 8  = 8
    //   The extra 6 bytes become padding at the end of this block.
    size_t aligned_size = ((size + align - 1) / align) * align;

    // total space needed for this allocation
    size_t total_needed = sizeof(ChunkHeader) + aligned_size;

    // if not enough space, throw
    if (offset_ + total_needed > capacity_) {
      throw std::bad_alloc();
    }

    // the ptr offset where this new chunk will live
    std::byte *slot = blk_.get() + offset_;

    // write the chunk into the blk with placement new
    // Size = the rounded size we calculated above
    // Magic = HeaderMagicId; this is how we mark "this block is live"
    // Memory = the address the caller will actually use (right after the
    // header)
    Chunk *chunk =
        new (slot) Chunk{ChunkHeader{aligned_size, ChunkHeader::HeaderMagicId},
                         slot + sizeof(ChunkHeader)};

    // bump the offset past this whole block
    offset_ += total_needed;

    return chunk->memory;
  }

  void Deallocate(std::byte *user_ptr) {
    if (user_ptr == nullptr) {
      return;
    }

    // get the header that sits immediately before the user pointer
    ChunkHeader *chunk_hdr =
        reinterpret_cast<ChunkHeader *>(user_ptr - sizeof(ChunkHeader));

    // only accept blocks that still carry our 'allocated' magic
    if (chunk_hdr->Magic != ChunkHeader::HeaderMagicId) {
      throw std::bad_alloc();
    }

    // bring back the offset
    offset_ -= chunk_hdr->Size + sizeof(ChunkHeader);

    // clear the header, so a second free of the same pointer will fail, as
    // magic will no longer be equal to HeaderMagicId
    chunk_hdr->Size = 0;
    chunk_hdr->Magic = 0;
  }

private:
  size_t offset_{};
  size_t capacity_{};
  std::unique_ptr<std::byte[]> blk_{nullptr};
};

} // namespace shyam
