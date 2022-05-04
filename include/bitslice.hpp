#ifndef INCLUDE_BITSLICE_HPP_
#define INCLUDE_BITSLICE_HPP_

#include <algorithm>
#include <bitset>
#include <cstddef>  // for size_t
#include <functional>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>  // for string


namespace bitslice
{
class bits
{
private:
    using Block = uint32_t;
    static constexpr std::size_t block_size =
        std::numeric_limits<Block>::digits;

    std::unique_ptr<Block[]> m_bitarr;
    std::size_t m_len;

    std::pair<std::size_t, std::size_t> get_num_block() const
    {
        return get_num_block(m_len);
    }
    std::pair<std::size_t, std::size_t> get_num_block(std::size_t s) const
    {
        return std::make_pair(s / block_size, s % block_size);
    }
    std::size_t get_arr_size() const { return get_arr_size(m_len); }
    std::size_t get_arr_size(std::size_t s) const
    {
        auto p = get_num_block(s);
        return p.first + static_cast<std::size_t>(p.second != 0);
    }

    bool check_range(std::size_t s, std::size_t e) const
    {
        return s < m_len && s >= e;
    }

    bits &do_operation(const bits &,
                       const std::function<Block(Block, Block)> &);

public:
    bits() = delete;
    explicit bits(std::size_t);
    explicit bits(std::size_t, uint64_t);
    explicit bits(const std::string &str);
    bits(std::initializer_list<bits>);

    bits(const bits &);             // copy constructor
    bits(bits &&);                  // move constructor
    bits &operator=(const bits &);  // copy assignment operator
    bits &operator=(bits &&);       // move assignment operator

    ~bits() = default;  // destructor

    void reverse();
    constexpr std::size_t get_size() const { return m_len; }

    std::string to_string() const { return to_string(m_len - 1, 0); }
    std::string to_string(std::size_t s) const { return to_string(s, 0); }
    std::string to_string(std::size_t, std::size_t) const;

    bits operator()(std::size_t s) const { return operator()(s, 0); }
    bits operator()(std::size_t, std::size_t) const;

    bool test(std::size_t pos) const;
    bool operator[](std::size_t pos) const;

    void set(std::size_t pos, bool val) const;

    bool operator==(const bits &) const;
    bits &operator+=(const bits &);
    bits &operator&=(const bits &);
    bits &operator|=(const bits &);
    bits &operator^=(const bits &);
    bits operator~() const;
};

bits::bits(std::size_t len) : m_bitarr(nullptr), m_len{len}
{
    if (len == 0) {
        throw std::invalid_argument("The length must not be zero");
    }

    std::size_t arr_size = get_arr_size();
    m_bitarr = std::make_unique<Block[]>(arr_size);
    for (std::size_t i = 0; i < arr_size; i++) {
        m_bitarr[i] = 0;
    }
}

bits::bits(std::size_t len, uint64_t val) : m_bitarr{nullptr}, m_len{len}
{
    if (len == 0) {
        throw std::invalid_argument("The length must not be zero");
    }
    std::size_t arr_size = get_arr_size();
    m_bitarr = std::make_unique<Block[]>(arr_size);
    for (std::size_t i = 0, j = 0; j < arr_size; i += block_size, j++) {
        m_bitarr[j] = val & ((1ULL << std::min(m_len - i, block_size)) - 1);
        val >>= block_size;
    }
}

bits::bits(std::initializer_list<bits> l) : m_len(0)
{
    *this = *l.begin();
    for (auto it = std::next(l.begin()); it != l.end(); it++) {
        *this += *it;
    }
}

bits::bits(const bits &other) : m_len{other.m_len}
{
    std::size_t arr_size = other.get_arr_size();
    m_bitarr = std::make_unique<Block[]>(arr_size);
    std::copy_n(other.m_bitarr.get(), arr_size, m_bitarr.get());
}

bits::bits(bits &&other) : m_bitarr{nullptr}, m_len{other.m_len}
{
    std::swap(other.m_bitarr, m_bitarr);
}

bits &bits::operator=(const bits &rhs)
{
    if (this == &rhs) {
        return *this;
    }

    std::size_t arr_size = rhs.get_arr_size();
    auto new_bitarr = std::make_unique<Block[]>(arr_size);
    m_bitarr = std::move(new_bitarr);
    m_len = rhs.m_len;

    std::copy_n(rhs.m_bitarr.get(), arr_size, m_bitarr.get());
    return *this;
}

bits &bits::operator=(bits &&rhs)
{
    if (this == &rhs) {
        return *this;
    }
    std::swap(rhs.m_bitarr, m_bitarr);
    std::swap(rhs.m_len, m_len);

    return *this;
}

bits::bits(const std::string &str) : m_len{str.length()}
{
    if (m_len == 0) {
        throw std::invalid_argument("The length must not be zero");
    }

    std::size_t arr_size = get_arr_size();
    std::string rev_str = str;
    std::reverse(rev_str.begin(), rev_str.end());

    m_bitarr = std::make_unique<Block[]>(arr_size);
    for (std::size_t i = 0, j = 0; i < str.length(); i += block_size, j++) {
        Block curblk = 0;
        for (std::size_t k = std::min(str.length() - i, block_size); k-- > 0;) {
            curblk = curblk << 1 | static_cast<Block>(rev_str[i + k] == '1');
        }
        m_bitarr[j] = curblk;
    }
}


bool bits::test(std::size_t pos) const
{
    if (pos >= m_len) {
        throw std::out_of_range("Position is out of range");
    }
    return this->operator[](pos);
}

bool bits::operator[](std::size_t pos) const
{
    auto p = get_num_block(pos);
    return static_cast<bool>((m_bitarr[p.first] >> p.second) & 1);
}

void bits::set(std::size_t pos, bool val) const
{
    if (pos >= m_len) {
        throw std::out_of_range("Position is out of range");
    }

    auto p = get_num_block(pos);
    const Block mask = static_cast<Block>(val) << p.second;

    if (m_bitarr[p.first] & (1 << p.second)) {
        m_bitarr[p.first] &= mask;
    } else {
        m_bitarr[p.first] |= mask;
    }
}


// void Bits::reverse()
//{
//     std::reverse(m_bitstr.begin(), m_bitstr.end());
// }
//

std::string bits::to_string(std::size_t s, std::size_t e) const
{
    if (!check_range(s, e)) {
        return "";
    }

    std::string bitstr;
    bitstr.reserve(s - e + 1);

    for (size_t i = s + 1; i-- > e;) {
        bitstr.append(1, operator[](i) ? '1' : '0');
    }
    return bitstr;
}

bits bits::operator()(std::size_t s, std::size_t e) const
{
    if (!check_range(s, e)) {
        throw std::out_of_range("range error");
    }

    const std::string b = to_string(s, e);
    return bits(b);
}

bool bits::operator==(const bits &rhs) const
{
    if (m_len != rhs.m_len) {
        return false;
    }

    std::size_t arr_size = get_arr_size(m_len);

    for (size_t i = 0; i < arr_size; i++) {
        if (m_bitarr[i] != rhs.m_bitarr[i]) {
            return false;
        }
    }
    return true;
}

// Bits &Bits::operator+=(const Bits &rhs)
//{
//    m_len += rhs.m_len;
//    m_bitstr += rhs.m_bitstr;
//
//    return (*this);
//}
//
//

bits &bits::operator+=(const bits &rhs)
{
    auto rhs_p = rhs.get_num_block();

    std::size_t new_arr_size = get_arr_size(m_len + rhs.m_len);
    auto new_bitarr = std::make_unique<Block[]>(new_arr_size);
    std::copy_n(rhs.m_bitarr.get(), rhs.get_arr_size(), new_bitarr.get());


    std::size_t offset = rhs_p.second;
    std::size_t init = rhs.get_arr_size() - 1;

    /* Handle residue bits */
    if (rhs_p.second != 0) {
        new_bitarr[init++] |= m_bitarr[0] << offset;
    }

    for (std::size_t i = init, j = 0; i < new_arr_size; i++, j++) {
        std::size_t upper_bits =
            j + 1 < this->get_arr_size() ? (m_bitarr[j + 1] << offset) : 0;
        std::size_t lower_bits = m_bitarr[j] >> (block_size - offset);
        new_bitarr[i] = upper_bits | lower_bits;
    }

    m_len = m_len + rhs.m_len;
    m_bitarr = std::move(new_bitarr);
    return *this;
}

bits &bits::do_operation(const bits &rhs,
                         const std::function<Block(Block, Block)> &op)
{
    std::size_t old_arr_size = this->get_arr_size();
    std::size_t rhs_arr_size = rhs.get_arr_size();
    std::size_t new_arr_size =
        get_arr_size(std::max(old_arr_size, rhs_arr_size));

    auto new_bitarr = std::make_unique<Block[]>(new_arr_size);
    for (size_t i = 0; i < new_arr_size; i++) {
        Block x = i >= old_arr_size ? 0 : m_bitarr[i];
        Block y = i >= rhs_arr_size ? 0 : rhs.m_bitarr[i];

        new_bitarr[i] = op(x, y);
    }

    m_bitarr = std::move(new_bitarr);
    m_len = std::max(m_len, rhs.m_len);

    return (*this);
}

bits &bits::operator&=(const bits &rhs)
{
    return do_operation(rhs, [](Block x, Block y) { return x & y; });
}

bits &bits::operator|=(const bits &rhs)
{
    return do_operation(rhs, [](Block x, Block y) { return x | y; });
}

bits &bits::operator^=(const bits &rhs)
{
    return do_operation(rhs, [](Block x, Block y) { return x ^ y; });
}

bits bits::operator~() const
{
    bits b(m_len);
    auto p = b.get_num_block();
    std::size_t arr_size = b.get_arr_size();

    for (std::size_t i = 0; i < arr_size; i++) {
        b.m_bitarr[i] = ~m_bitarr[i];
    }

    if (p.second != 0) {
        b.m_bitarr[arr_size - 1] =
            (b.m_bitarr[arr_size - 1] << (block_size - p.second)) >>
            (block_size - p.second);
    }

    return b;
}


bits operator+(bits lhs, const bits &rhs)
{
    lhs += rhs;
    return lhs;
}

bits operator&(bits lhs, const bits &rhs)
{
    lhs &= rhs;
    return lhs;
}
bits operator|(bits lhs, const bits &rhs)
{
    lhs |= rhs;
    return lhs;
}
bits operator^(bits lhs, const bits &rhs)
{
    lhs ^= rhs;
    return lhs;
}

std::ostream &operator<<(std::ostream &os, const bits &b)
{
    os << b.to_string();
    return os;
}


namespace literals
{

// clang-format off

// Generated with:
// for i in {1..64}; do
//     printf 'bits operator"" _%d(unsigned long long n) { return bits{%d, n}; }\n' "$i" "$i"
// done

bits operator"" _1(unsigned long long n) { return bits{1, n}; }
bits operator"" _2(unsigned long long n) { return bits{2, n}; }
bits operator"" _3(unsigned long long n) { return bits{3, n}; }
bits operator"" _4(unsigned long long n) { return bits{4, n}; }
bits operator"" _5(unsigned long long n) { return bits{5, n}; }
bits operator"" _6(unsigned long long n) { return bits{6, n}; }
bits operator"" _7(unsigned long long n) { return bits{7, n}; }
bits operator"" _8(unsigned long long n) { return bits{8, n}; }
bits operator"" _9(unsigned long long n) { return bits{9, n}; }
bits operator"" _10(unsigned long long n) { return bits{10, n}; }
bits operator"" _11(unsigned long long n) { return bits{11, n}; }
bits operator"" _12(unsigned long long n) { return bits{12, n}; }
bits operator"" _13(unsigned long long n) { return bits{13, n}; }
bits operator"" _14(unsigned long long n) { return bits{14, n}; }
bits operator"" _15(unsigned long long n) { return bits{15, n}; }
bits operator"" _16(unsigned long long n) { return bits{16, n}; }
bits operator"" _17(unsigned long long n) { return bits{17, n}; }
bits operator"" _18(unsigned long long n) { return bits{18, n}; }
bits operator"" _19(unsigned long long n) { return bits{19, n}; }
bits operator"" _20(unsigned long long n) { return bits{20, n}; }
bits operator"" _21(unsigned long long n) { return bits{21, n}; }
bits operator"" _22(unsigned long long n) { return bits{22, n}; }
bits operator"" _23(unsigned long long n) { return bits{23, n}; }
bits operator"" _24(unsigned long long n) { return bits{24, n}; }
bits operator"" _25(unsigned long long n) { return bits{25, n}; }
bits operator"" _26(unsigned long long n) { return bits{26, n}; }
bits operator"" _27(unsigned long long n) { return bits{27, n}; }
bits operator"" _28(unsigned long long n) { return bits{28, n}; }
bits operator"" _29(unsigned long long n) { return bits{29, n}; }
bits operator"" _30(unsigned long long n) { return bits{30, n}; }
bits operator"" _31(unsigned long long n) { return bits{31, n}; }
bits operator"" _32(unsigned long long n) { return bits{32, n}; }
bits operator"" _33(unsigned long long n) { return bits{33, n}; }
bits operator"" _34(unsigned long long n) { return bits{34, n}; }
bits operator"" _35(unsigned long long n) { return bits{35, n}; }
bits operator"" _36(unsigned long long n) { return bits{36, n}; }
bits operator"" _37(unsigned long long n) { return bits{37, n}; }
bits operator"" _38(unsigned long long n) { return bits{38, n}; }
bits operator"" _39(unsigned long long n) { return bits{39, n}; }
bits operator"" _40(unsigned long long n) { return bits{40, n}; }
bits operator"" _41(unsigned long long n) { return bits{41, n}; }
bits operator"" _42(unsigned long long n) { return bits{42, n}; }
bits operator"" _43(unsigned long long n) { return bits{43, n}; }
bits operator"" _44(unsigned long long n) { return bits{44, n}; }
bits operator"" _45(unsigned long long n) { return bits{45, n}; }
bits operator"" _46(unsigned long long n) { return bits{46, n}; }
bits operator"" _47(unsigned long long n) { return bits{47, n}; }
bits operator"" _48(unsigned long long n) { return bits{48, n}; }
bits operator"" _49(unsigned long long n) { return bits{49, n}; }
bits operator"" _50(unsigned long long n) { return bits{50, n}; }
bits operator"" _51(unsigned long long n) { return bits{51, n}; }
bits operator"" _52(unsigned long long n) { return bits{52, n}; }
bits operator"" _53(unsigned long long n) { return bits{53, n}; }
bits operator"" _54(unsigned long long n) { return bits{54, n}; }
bits operator"" _55(unsigned long long n) { return bits{55, n}; }
bits operator"" _56(unsigned long long n) { return bits{56, n}; }
bits operator"" _57(unsigned long long n) { return bits{57, n}; }
bits operator"" _58(unsigned long long n) { return bits{58, n}; }
bits operator"" _59(unsigned long long n) { return bits{59, n}; }
bits operator"" _60(unsigned long long n) { return bits{60, n}; }
bits operator"" _61(unsigned long long n) { return bits{61, n}; }
bits operator"" _62(unsigned long long n) { return bits{62, n}; }
bits operator"" _63(unsigned long long n) { return bits{63, n}; }
bits operator"" _64(unsigned long long n) { return bits{64, n}; }
// clang-format on

bits operator"" _b(unsigned long long n)
{
    /* Set default length to 64 */
    return bits{64, n};
}


bits operator"" _b(const char *str, std::size_t sz)
{
    return bits(std::string{str, sz});
}

}  // namespace literals

}  // namespace bitslice


#endif  // INCLUDE_BITS_HPP_
