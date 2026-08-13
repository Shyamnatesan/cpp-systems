#include <memory>
#include <stdexcept>

namespace shyam {

template <typename T> struct custom_deleter {
  void operator()(T *pointer) const { delete pointer; }
};

template <typename T, typename custom_deleter = custom_deleter<T>>
class unique_ptr {
public:
  // default constructor – creates an empty unique_ptr that owns nothing
  unique_ptr() {}

  // taking ownership of a raw pointer
  unique_ptr(T *pointer) : ptr_(pointer) {}

  // unique_ptr is move-only. Copying is not allowed.
  unique_ptr(const unique_ptr &) = delete;
  unique_ptr &operator=(const unique_ptr &) = delete;

  // move constructor
  // steal the resource from 'other' and leave 'other' empty
  unique_ptr(unique_ptr &&other) noexcept {
    ptr_ = other.ptr_;
    other.ptr_ =
        nullptr; // very important: source must be left in a valid state
  }

  // move assignment
  // 1. delete whatever we currently own
  // 2. steal the resource from 'other'
  // 3. leave 'other' empty
  unique_ptr &operator=(unique_ptr &&other) noexcept {
    custom_deleter{}(ptr_); // release our old resource
    ptr_ = other.ptr_;
    other.ptr_ = nullptr;
    return *this;
  }

  // destructor – release the managed object
  ~unique_ptr() {
    custom_deleter{}(ptr_);
    ptr_ = nullptr;
  }

  // give up ownership and return the raw pointer.
  // after this call the unique_ptr is empty.
  T *release() {
    auto temp = ptr_;
    ptr_ = nullptr;
    return temp;
  }

  // delete the current object and optionally take ownership of a new one
  void reset(T *pointer = nullptr) {
    custom_deleter{}(ptr_);
    ptr_ = pointer;
  }

  // does this unique_ptr currently own a resource?
  bool is_owning() const { return ptr_ != nullptr; }

  // dereference operators
  T &operator*() const { return *ptr_; }
  T *operator->() const { return ptr_; }

  // allow using the unique_ptr in boolean contexts
  // e.g. if (ptr) { ... }
  operator bool() const { return is_owning(); }

private:
  T *ptr_{nullptr};
};

} // namespace shyam
