#include <cstddef>
#include <memory>
#include <new>

namespace shyam {

struct Message {
  int id;
  int size;
};

/*
 * How the free list works without any extra memory
 * ===============================================
 *
 * Normally if you want a free list you need somewhere to store the "next"
 * pointers.  That usually means extra memory per slot.  We don't do that here.
 *
 * The trick is simple: when a slot is free it is not holding a Message.
 * Those bytes are just sitting there doing nothing.  So we reuse them.
 * While the slot is free we treat the beginning of that storage as an int
 * that holds the index of the next free slot.
 *
 * When we allocate the slot we overwrite those same bytes with a real
 * Message.  When we free it again we put an index back into the same place.
 *
 * So the free list is literally threaded through the free slots themselves.
 * The only extra thing we keep is a single bool that tells us whether the
 * slot is currently free or in use.  That's allowed by the problem.
 *
 * We also keep one integer outside (fl_head_) which is just the index of
 * the first free slot.  -1 means "no free slots left".
 *
 * allocate = pop from the front of that list
 * release  = push onto the front of that list
 * Both are O(1).  No searching ever.
 *
 * On construction we link the slots in order: 0 → 1 → 2 → … → -1
 * so the first few allocations give you handles 0, 1, 2, …
 * reset() just rebuilds that same chain.
 */
class MemPool {
public:
  // allocate the block, and construct the slots
  MemPool(int capacity)
      : capacity_(static_cast<size_t>(capacity)),
        blk_(std::make_unique<std::byte[]>(capacity_ * sizeof(Slot))) {
    ConstructSlots();
  }

  int allocate(Message message) {
    // check if head is valid, if -1, then all slots are in use
    auto free_slot_idx = fl_head_;
    if (free_slot_idx == -1) {
      return -1;
    }

    // turn the index into an actual pointer into our byte buffer
    Slot *free_slot =
        reinterpret_cast<Slot *>(blk_.get() + free_slot_idx * sizeof(Slot));

    // this slot is free right now, so its payload is holding the next free
    // index.  grab that index and make it the new head.  that's the pop.
    fl_head_ = free_slot->payload.free_slot.next_free_idx;

    // now the slot becomes live.  write the caller's message into it and
    // mark it as not free anymore.
    free_slot->payload.msg = message;
    free_slot->is_free = false;

    --free_slot_count;
    return free_slot_idx;
  }

  bool release(int handle) {
    // check if handle is within the bounds of the pool
    if (handle < 0 || static_cast<size_t>(handle) >= capacity_) {
      return false;
    }

    // read the slot at the handle
    Slot *released =
        reinterpret_cast<Slot *>(blk_.get() + handle * sizeof(Slot));

    // check if already released
    if (released->is_free) {
      return false;
    }

    // this handle will be the next-to-be-allocated slot.
    // so this handle will be the head, and the current head will be swapped
    // inside this slot.
    // (we are pushing onto the front of the free list)
    released->payload.free_slot.next_free_idx = fl_head_;
    fl_head_ = handle;

    // mark the slot as free
    released->is_free = true;
    free_slot_count++;
    return true;
  }

  Message *get(int handle) {
    // check if handle is within the bounds of the pool
    if (handle < 0 || static_cast<size_t>(handle) >= capacity_) {
      return nullptr;
    }

    // read the slot at the handle
    auto slot = reinterpret_cast<Slot *>(blk_.get() + handle * sizeof(Slot));

    // This slot should not be free, as get is only called after allocate
    if (slot->is_free) {
      return nullptr;
    }

    // return a pointer to the slot's message
    auto msg = &slot->payload.msg;
    return msg;
  }

  int available() const {
    // return number of free slots available at any given time
    return free_slot_count;
  }

  void reset() {
    // simply call constructSlots which updates the slots in-place with free
    // slots.
    ConstructSlots();
  }

private:
  // A slot can either be a FreeSlot or a Message.
  // We also need a marker for distinguishing between free and in-use slots.
  // The union lets both of them live in the exact same bytes.  Only one is
  // alive at a time; the bool tells us which one.
  struct FreeSlot {
    int next_free_idx;
  };
  union Payload {
    Message msg;
    FreeSlot free_slot;
  };
  struct Slot {
    bool is_free{};
    Payload payload;
    Slot(int next_free_idx) {
      is_free = true;
      // we are constructing a free slot, so we write the free-list side
      // of the union
      payload = Payload{
          .free_slot =
              FreeSlot{
                  .next_free_idx = next_free_idx,
              },
      };
    }
  };

  // capacity of the memory pool
  size_t capacity_{};

  // number of free slots
  int free_slot_count{};

  // head of the free list. this represents the next_to_be_allocated slot's
  // index
  int fl_head_{};

  // pointer to the block.
  // even better design is to just hold a vector of slots in here, instead of a
  // pointer.
  std::unique_ptr<std::byte[]> blk_{nullptr};

  void ConstructSlots() {
    // Go through the byte buffer and construct FreeSlot in-place using
    // placement-new.
    // We link them in order: 0 → 1 → 2 → … → -1
    // so the first allocations give us the nice ascending handles.
    for (size_t i{}; i < capacity_; ++i) {
      int next_free_slot_idx =
          static_cast<int>((i == capacity_ - 1) ? -1 : i + 1);
      new (blk_.get() + i * sizeof(Slot)) Slot(next_free_slot_idx);
    }
    // update the head of free list and free slot count
    free_slot_count = capacity_;
    fl_head_ = 0;
  }
};

} // namespace shyam
