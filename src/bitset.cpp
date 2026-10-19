#include "bitset.h"

#include <algorithm>
#include <limits>
#include <ostream>

bitset::bitset()
    : size_(0)
    , data_(nullptr) {}

bitset::bitset(std::size_t size, bool value)
    : size_(size)
    , data_(new word_type[calc_size_in_words_()]) {
  std::fill_n(data_, calc_size_in_words_(), value ? ~static_cast<word_type>(0) : 0);
}

bitset::bitset(const bitset& other)
    : size_(other.size_)
    , data_(new word_type[calc_size_in_words_()]) {
  std::copy(other.begin(), other.end(), this->begin());
}

bitset::bitset(std::string_view str)
    : size_(str.length())
    , data_(new word_type[calc_size_in_words_()]) {
  std::transform(str.begin(), str.end(), this->begin(), [](const char& c) { return c == '1'; });
}

bitset::bitset(const const_view& other)
    : size_(other.size())
    , data_(new word_type[calc_size_in_words_()]) {
  std::transform(other.begin(), other.end(), this->begin(), [](auto r) { return static_cast<bool>(r); });
}

bitset::bitset(const_iterator first, const_iterator last)
    : size_(last - first)
    , data_(new word_type[calc_size_in_words_()]) {
  std::transform(first, last, this->begin(), [](auto r) { return static_cast<bool>(r); });
}

bitset& bitset::operator=(const bitset& other) & {
  if (this == &other) {
    return *this;
  }
  bitset copy = other;
  this->swap(copy);
  return *this;
}

bitset& bitset::operator=(std::string_view str) & {
  bitset tmp(str);
  swap(tmp);
  return *this;
}

bitset& bitset::operator=(const const_view& other) & {
  bitset tmp(other);
  swap(tmp);
  return *this;
}

bitset::~bitset() {
  delete[] data_;
}

bitset& bitset::operator&=(const const_view& other) & {
  static_cast<view>(*this) &= other;
  return *this;
}

bitset& bitset::operator|=(const const_view& other) & {
  static_cast<view>(*this) |= other;
  return *this;
}

bitset& bitset::operator^=(const const_view& other) & {
  static_cast<view>(*this) ^= other;
  return *this;
}

bitset& bitset::operator<<=(std::size_t count) & {
  if (count == 0) {
    return *this;
  }
  bitset temp(this->size_ + count, false);
  std::size_t words_to_process = this->size_ / word_size;

  std::copy_n(this->data_, words_to_process, temp.data_);
  for (std::size_t i = words_to_process * word_size; i < this->size_; ++i) {
    temp[i] = static_cast<bool>((*this)[i]);
  }

  swap(temp);
  return *this;
}

bitset& bitset::operator>>=(std::size_t count) & {
  if (count == 0) {
    return *this;
  }
  bitset temp(this->size_ - count > this->size_ ? 0 : this->size_ - count, false);
  std::size_t words_to_process = temp.size_ / word_size;

  std::copy_n(this->data_, words_to_process, temp.data_);
  for (std::size_t i = words_to_process * word_size; i < temp.size_; ++i) {
    temp[i] = static_cast<bool>((*this)[i]);
  }

  swap(temp);
  return *this;
}

void bitset::flip() & {
  static_cast<view>(*this).flip();
}

bitset& bitset::set() & {
  static_cast<view>(*this).set();
  return *this;
}

bitset& bitset::reset() & {
  static_cast<view>(*this).reset();
  return *this;
}

bitset::reference bitset::operator[](std::size_t index) {
  return static_cast<view>(*this)[index];
}

bitset::const_reference bitset::operator[](std::size_t index) const {
  return static_cast<const_view>(*this)[index];
}

bitset::iterator bitset::begin() {
  return {data_, 0};
}

bitset::const_iterator bitset::begin() const {
  return {data_, 0};
}

bitset::iterator bitset::end() {
  return {data_, size_};
}

bitset::const_iterator bitset::end() const {
  return {data_, size_};
}

bool bitset::all() const {
  return static_cast<const_view>(*this).all();
}

bool bitset::any() const {
  return static_cast<const_view>(*this).any();
}

std::size_t bitset::count() const {
  return static_cast<const_view>(*this).count();
}

void bitset::swap(bitset& other) {
  std::swap(size_, other.size_);
  std::swap(data_, other.data_);
}

std::size_t bitset::size() const {
  return size_;
}

bool bitset::empty() const {
  return this->size_ == 0;
}

bitset::view bitset::subview(std::size_t offset, std::size_t count) {
  return static_cast<view>(*this).subview(offset, count);
}

bitset::const_view bitset::subview(std::size_t offset, std::size_t count) const {
  return static_cast<const_view>(*this).subview(offset, count);
}

bitset::operator const_view() const {
  return {this->begin(), this->end()};
}

bitset::operator view() {
  return {this->begin(), this->end()};
}

std::size_t bitset::calc_size_in_words_() const {
  return (size_ + std::numeric_limits<word_type>::digits - 1) / std::numeric_limits<word_type>::digits;
}

const bitset::word_type* bitset::get_iterator_data_(const const_iterator& it) {
  return it.data_;
}

template <typename F>
bitset bitset::binary_op(const const_view& lhs, const const_view& rhs, F op) {
  bitset res(lhs);
  static_cast<view>(res).binary_op(rhs, op);
  return res;
}

bitset operator&(const bitset& lhs, const bitset& rhs) {
  return static_cast<bitset::const_view>(lhs) & static_cast<bitset::const_view>(rhs);
}

bitset operator|(const bitset& lhs, const bitset& rhs) {
  return static_cast<bitset::const_view>(lhs) | static_cast<bitset::const_view>(rhs);
}

bitset operator^(const bitset& lhs, const bitset& rhs) {
  return static_cast<bitset::const_view>(lhs) ^ static_cast<bitset::const_view>(rhs);
}

bitset operator~(const bitset& bs) {
  return ~static_cast<bitset::const_view>(bs);
}

bitset operator<<(const bitset& bs, std::size_t count) {
  bitset res = bs;
  res <<= count;
  return res;
}

bitset operator>>(const bitset& bs, std::size_t count) {
  bitset res = bs;
  res >>= count;
  return res;
}

bool operator==(const bitset& left, const bitset& right) {
  return static_cast<bitset::const_view>(left) == static_cast<bitset::const_view>(right);
}

bool operator!=(const bitset& left, const bitset& right) {
  return !(left == right);
}

void swap(bitset& lhs, bitset& rhs) {
  lhs.swap(rhs);
}

std::string to_string(const bitset& bs) {
  return to_string(static_cast<bitset::const_view>(bs));
}

std::ostream& operator<<(std::ostream& out, const bitset& bs) {
  return operator<<(out, static_cast<bitset::const_view>(bs));
}

bitset operator&(const bitset_view<bitset::const_iterator>& lhs, const bitset_view<bitset::const_iterator>& rhs) {
  return bitset::binary_op(lhs, rhs, std::bit_and{});
}

bitset operator|(const bitset_view<bitset::const_iterator>& lhs, const bitset_view<bitset::const_iterator>& rhs) {
  return bitset::binary_op(lhs, rhs, std::bit_or{});
}

bitset operator^(const bitset_view<bitset::const_iterator>& lhs, const bitset_view<bitset::const_iterator>& rhs) {
  return bitset::binary_op(lhs, rhs, std::bit_xor{});
}

bitset operator~(const bitset_view<bitset::const_iterator>& bv) {
  bitset res(bv);
  res.flip();
  return res;
}

bitset operator<<(const bitset_view<bitset::const_iterator>& bv, std::size_t count) {
  bitset res(bv);
  return res <<= count;
}

bitset operator>>(const bitset_view<bitset::const_iterator>& bv, std::size_t count) {
  bitset res(bv);
  return res >>= count;
}
