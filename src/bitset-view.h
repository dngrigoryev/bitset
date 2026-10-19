#pragma once

#include "bitset-iterator.h"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <ostream>
#include <string>

// T can only be an bitset::iterator or bitset::const_iterator
template <typename T>
class bitset_view {
  friend class bitset;

public:
  using word_type = std::uint64_t;
  using value_type = bool;
  using reference = typename T::reference;
  using const_reference = bitset_reference<const word_type>;
  using iterator = T;
  using const_iterator = bitset_iterator<const word_type>;
  using view = bitset_view;
  using const_view = bitset_view<const_iterator>;

  static constexpr std::size_t npos = -1;
  static constexpr std::size_t word_size = std::numeric_limits<word_type>::digits;

  bitset_view() = default;
  bitset_view(const bitset_view& other) = default;
  bitset_view& operator=(const bitset_view& other) = default;
  ~bitset_view() = default;

  bitset_view(const iterator& left, const iterator& right)
      : begin_(left)
      , size_(right - left) {}

  operator const_view() const {
    return {this->begin(), this->end()};
  }

  view operator&=(const const_view& other) const {
    return binary_op(other, std::bit_and{});
  }

  view operator|=(const const_view& other) const {
    return binary_op(other, std::bit_or{});
  }

  view operator^=(const const_view& other) const {
    return binary_op(other, std::bit_xor{});
  }

  view flip() const {
    struct op {
      word_type operator()(const word_type& n) {
        return ~n;
      }

      bool operator()(bool b) {
        return !b;
      }
    };

    return unary_op(op{});
  }

  void set() const {
    struct op {
      word_type operator()(const word_type&) {
        return ~static_cast<word_type>(0);
      }

      bool operator()(bool) {
        return true;
      }
    };

    unary_op(op{});
  }

  void reset() const {
    struct op {
      word_type operator()(const word_type&) {
        return 0;
      }

      bool operator()(bool) {
        return false;
      }
    };

    unary_op(op{});
  }

  reference operator[](std::size_t index) const {
    return *(begin_ + index);
  }

  iterator begin() const {
    return begin_;
  }

  iterator end() const {
    return begin_ + size_;
  }

  bool all() const {
    return std::all_of(this->begin(), this->end(), std::identity{});
  }

  bool any() const {
    return std::any_of(this->begin(), this->end(), std::identity{});
  }

  std::size_t count() const {
    return std::count(this->begin(), this->end(), true);
  }

  void swap(view& other) noexcept {
    std::swap(this->begin_, other.begin_);
    std::swap(this->size_, other.size_);
  }

  std::size_t size() const {
    return size_;
  }

  bool empty() const {
    return this->size_ == 0;
  }

  view subview(std::size_t offset = 0, std::size_t count = npos) const {
    if (offset > this->size_) {
      return {this->end(), this->end()};
    }
    if (offset + count > this->size_ || offset + count < offset) {
      return {this->begin() + static_cast<ptrdiff_t>(offset), this->end()};
    }
    return {
        this->begin() + static_cast<ptrdiff_t>(offset),
        this->begin() + static_cast<ptrdiff_t>(offset) + static_cast<ptrdiff_t>(count)
    };
  }

  bool operator==(const const_view& rhs) const {
    if (this->size() != rhs.size()) {
      return false;
    }
    std::size_t words_to_process = this->size_ / word_size;
    return std::equal(this->begin().data_, this->begin().data_ + words_to_process, rhs.begin().data_) &&
           std::equal(
               this->begin() + words_to_process * word_size,
               this->end(),
               rhs.begin() + words_to_process * word_size
           );
  }

  bool operator!=(const const_view& rhs) const {
    return !(*this == rhs);
  }

private:
  iterator begin_;
  std::size_t size_ = 0;

  template <typename F>
  view unary_op(F op) const {
    std::size_t words_to_process = this->size_ / word_size;

    std::transform(this->begin().data_, this->begin().data_ + words_to_process, this->begin().data_, op);
    for (std::size_t i = words_to_process * word_size; i < this->size_; ++i) {
      (*this)[i] = op.operator()(static_cast<bool>((*this)[i]));
    }

    return *this;
  }

  template <typename F>
  view binary_op(const const_view& other, F op) const {
    std::size_t words_to_process = this->size_ / word_size;

    std::transform(
        this->begin().data_,
        this->begin().data_ + words_to_process,
        other.begin().data_,
        this->begin().data_,
        op
    );
    for (std::size_t i = words_to_process * word_size; i < this->size_; ++i) {
      (*this)[i] = op.operator()(static_cast<bool>((*this)[i]), static_cast<bool>(other[i]));
    }

    return *this;
  }
};

template <typename T>
void swap(bitset_view<T>& lhs, bitset_view<T>& rhs) noexcept {
  lhs.swap(rhs);
}

template <typename T>
std::string to_string(const bitset_view<T>& bv) {
  std::string res(bv.size(), '\0');
  auto it = bv.begin();
  std::size_t i = 0;
  while (it != bv.end()) {
    res[i++] = (*it ? '1' : '0');
    ++it;
  }
  return res;
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const bitset_view<T>& bv) {
  auto it = bv.begin();
  while (it != bv.end()) {
    out.put(*it ? '1' : '0');
    ++it;
  }
  return out;
}
