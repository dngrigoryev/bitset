#pragma once

#include <cstddef>

// T can only be a bitset::word_type or const bitset::word_type
template <typename T>
class bitset_reference {
  template <typename TT>
  friend class bitset_iterator;

public:
  bitset_reference() = delete;
  bitset_reference(const bitset_reference& other) = default;
  bitset_reference& operator=(const bitset_reference& other) = default;
  ~bitset_reference() = default;

  operator bitset_reference<const T>() const {
    return bitset_reference<const T>(*this);
  }

  operator bool() const {
    return static_cast<bool>(*byte_ & get_bit_mask_());
  }

  bitset_reference& operator=(const bool value) {
    if (value) {
      *byte_ |= get_bit_mask_();
    } else {
      *byte_ &= ~get_bit_mask_();
    }
    return *this;
  }

  const bitset_reference& operator=(const bool value) const {
    if (value) {
      *byte_ |= get_bit_mask_();
    } else {
      *byte_ &= ~get_bit_mask_();
    }
    return *this;
  }

  void flip() const {
    *byte_ ^= get_bit_mask_();
  }

private:
  // idx is in little-endian inside the T
  bitset_reference(T* b_, const size_t idx)
      : byte_(b_)
      , idx_(idx) {}

  T* byte_;
  std::size_t idx_;

  T get_bit_mask_() const {
    return static_cast<T>(1) << idx_;
  }
};
