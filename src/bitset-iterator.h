#pragma once

#include "bitset-reference.h"

#include <iterator>
#include <limits>

// T can only be a bitset::word_type or const bitset::word_type
template <typename T>
class bitset_iterator {
  friend class bitset;

  template <typename U>
  friend class bitset_view;

  friend class bitset_iterator<std::remove_const_t<T>>;
  friend class bitset_iterator<const std::remove_const_t<T>>;

public:
  using value_type = bool;
  using difference_type = std::ptrdiff_t;
  using reference = bitset_reference<T>;
  using pointer = void;
  using iterator_category = std::random_access_iterator_tag;

  bitset_iterator() = default;
  bitset_iterator(const bitset_iterator& other) = default;
  bitset_iterator& operator=(const bitset_iterator& other) = default;
  ~bitset_iterator() = default;

private:
  bitset_iterator(T* data, const size_t idx)
      : data_(data)
      , idx_(idx) {}

public:
  operator bitset_iterator<const T>() const {
    return {this->data_, this->idx_};
  }

  reference operator*() const {
    return {data_ + get_word_number_(), get_bit_number()};
  }

  reference operator[](const difference_type& n) const {
    return *(*this + n);
  }

  pointer operator->() = delete;

  bitset_iterator& operator++() {
    ++idx_;
    return *this;
  }

  bitset_iterator operator++(int) {
    bitset_iterator tmp = *this;
    ++*this;
    return tmp;
  }

  bitset_iterator& operator--() {
    --idx_;
    return *this;
  }

  bitset_iterator operator--(int) {
    bitset_iterator tmp = *this;
    --*this;
    return tmp;
  }

  bitset_iterator& operator+=(const difference_type& n) {
    idx_ += n;
    return *this;
  }

  bitset_iterator& operator-=(const difference_type& n) {
    return *this += -n;
  }

  friend bitset_iterator operator+(const bitset_iterator& lhs, const difference_type& n) {
    bitset_iterator tmp = lhs;
    return tmp += n;
  }

  friend bitset_iterator operator+(const difference_type& n, const bitset_iterator& rhs) {
    bitset_iterator tmp = rhs;
    return tmp += n;
  }

  friend bitset_iterator operator-(const bitset_iterator& lhs, const difference_type& n) {
    bitset_iterator tmp = lhs;
    return tmp -= n;
  }

  friend difference_type operator-(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return lhs.idx_ - rhs.idx_;
  }

  friend bool operator==(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return lhs.idx_ == rhs.idx_;
  }

  friend bool operator!=(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return !(lhs == rhs);
  }

  friend bool operator<(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return rhs - lhs > 0;
  }

  friend bool operator>(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return rhs < lhs;
  }

  friend bool operator>=(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return !(lhs < rhs);
  }

  friend bool operator<=(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return !(lhs > rhs);
  }

private:
  T* data_;
  std::size_t idx_;

  size_t get_word_number_() const {
    return idx_ / std::numeric_limits<T>::digits;
  }

  // bit number is in little-endian inside the T
  size_t get_bit_number() const {
    return std::numeric_limits<T>::digits - (idx_ % std::numeric_limits<T>::digits) - 1;
  }
};
