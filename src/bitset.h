#pragma once

#include "bitset-view.h"

#include <cstddef>
#include <cstdint>
#include <string_view>

class bitset {
public:
  using word_type = std::uint64_t;
  using value_type = bool;
  using reference = bitset_reference<word_type>;
  using const_reference = bitset_reference<const word_type>;
  using iterator = bitset_iterator<word_type>;
  using const_iterator = bitset_iterator<const word_type>;
  using view = bitset_view<iterator>;
  using const_view = bitset_view<const_iterator>;

  static constexpr std::size_t npos = -1;
  static constexpr std::size_t word_size = std::numeric_limits<word_type>::digits;

  bitset();
  bitset(std::size_t size, bool value);
  bitset(const bitset& other);
  explicit bitset(std::string_view str);
  explicit bitset(const const_view& other);
  bitset(const_iterator first, const_iterator last);

  bitset& operator=(const bitset& other) &;
  bitset& operator=(std::string_view str) &;
  bitset& operator=(const const_view& other) &;

  ~bitset();

  bitset& operator&=(const const_view& other) &;
  bitset& operator|=(const const_view& other) &;
  bitset& operator^=(const const_view& other) &;
  bitset& operator<<=(std::size_t count) &;
  bitset& operator>>=(std::size_t count) &;
  void flip() &;

  bitset& set() &;
  bitset& reset() &;

  reference operator[](std::size_t index);
  const_reference operator[](std::size_t index) const;

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  bool all() const;
  bool any() const;
  std::size_t count() const;

  void swap(bitset& other);

  std::size_t size() const;
  bool empty() const;

  view subview(std::size_t offset = 0, std::size_t count = npos);
  const_view subview(std::size_t offset = 0, std::size_t count = npos) const;

  operator const_view() const;
  operator view();

  friend bitset operator&(const bitset_view<const_iterator>& lhs, const bitset_view<const_iterator>& rhs);
  friend bitset operator|(const bitset_view<const_iterator>& lhs, const bitset_view<const_iterator>& rhs);
  friend bitset operator^(const bitset_view<const_iterator>& lhs, const bitset_view<const_iterator>& rhs);
  friend bitset operator~(const bitset_view<const_iterator>& bv);
  friend bitset operator<<(const bitset_view<const_iterator>& bv, std::size_t count);
  friend bitset operator>>(const bitset_view<const_iterator>& bv, std::size_t count);

private:
  std::size_t size_;
  word_type* data_;

  std::size_t calc_size_in_words_() const;

  static const word_type* get_iterator_data_(const const_iterator& it);

  template <typename F>
  static bitset binary_op(const bitset_view<const_iterator>& lhs, const bitset_view<const_iterator>& rhs, F op);
};

bitset operator&(const bitset& lhs, const bitset& rhs);
bitset operator|(const bitset& lhs, const bitset& rhs);
bitset operator^(const bitset& lhs, const bitset& rhs);
bitset operator~(const bitset& bs);
bitset operator<<(const bitset& bs, std::size_t count);
bitset operator>>(const bitset& bs, std::size_t count);

bool operator==(const bitset& left, const bitset& right);
bool operator!=(const bitset& left, const bitset& right);

void swap(bitset& lhs, bitset& rhs);

std::string to_string(const bitset& bs);
std::ostream& operator<<(std::ostream& out, const bitset& bs);

// operations on views that require bitset definition
bitset operator&(const bitset_view<bitset::const_iterator>& lhs, const bitset_view<bitset::const_iterator>& rhs);
bitset operator|(const bitset_view<bitset::const_iterator>& lhs, const bitset_view<bitset::const_iterator>& rhs);
bitset operator^(const bitset_view<bitset::const_iterator>& lhs, const bitset_view<bitset::const_iterator>& rhs);
bitset operator~(const bitset_view<bitset::const_iterator>& bv);
bitset operator<<(const bitset_view<bitset::const_iterator>& bv, std::size_t count);
bitset operator>>(const bitset_view<bitset::const_iterator>& bv, std::size_t count);
