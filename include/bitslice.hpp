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

    void trim_last_block();

public:
    bits() = delete;
    explicit bits(std::size_t);
    explicit bits(std::size_t, uint64_t);
    explicit bits(const std::string &str);
    bits(std::initializer_list<bits>);

    static bits ones(std::size_t);
    static bits zeros(std::size_t);

    bits(const bits &);             // copy constructor
    bits(bits &&);                  // move constructor
    bits &operator=(const bits &);  // copy assignment operator
    bits &operator=(bits &&);       // move assignment operator

    ~bits() = default;  // destructor

    void reverse();
    constexpr std::size_t get_size() const { return m_len; }

    bits &repeat(uint64_t);
    bits &append(const bits &);

    std::string to_string() const { return to_string(m_len - 1, 0); }
    std::string to_string(std::size_t s) const { return to_string(s, 0); }
    std::string to_string(std::size_t, std::size_t) const;

    bits operator()(std::size_t s) const { return operator()(s, 0); }
    bits operator()(std::size_t, std::size_t) const;


    uint64_t get_nbits(uint64_t pos, std::size_t digit = block_size);
    void set_nbits(uint64_t val, uint64_t pos, std::size_t digit = block_size);

    bool test(std::size_t pos) const;
    bool operator[](std::size_t pos) const;

    void set(std::size_t pos, bool val) const;

    bool operator==(const bits &) const;

    bits &operator>>=(uint64_t);
    bits &operator<<=(uint64_t);
    bits &operator*=(uint64_t);
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
        this->append(*it);
    }
}

bits bits::ones(std::size_t nbits)
{
    bits b{nbits};
    std::fill(b.m_bitarr.get(), b.m_bitarr.get() + b.get_arr_size(), -1);

    b.trim_last_block();
    return b;
}

bits bits::zeros(std::size_t nbits)
{
    bits b{nbits};
    std::fill(b.m_bitarr.get(), b.m_bitarr.get() + b.get_arr_size(), 0);

    return b;
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

bits &bits::repeat(uint64_t times)
{
    if (times == 0) {
        throw std::invalid_argument("The times must not be zero");
    }

    /* Save the origin value; */
    const bits b{*this};

    for (uint64_t i = 0; i < times - 1; i++) {
        this->append(b);
    }
    return *this;
}

bits &bits::append(const bits &rhs)
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

    for (std::size_t i = init, j = block_size - offset; i < new_arr_size;
         i++, j += block_size) {
        new_bitarr[i] = get_nbits(j, block_size);
    }

    m_len = m_len + rhs.m_len;
    m_bitarr = std::move(new_bitarr);
    return *this;
}

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

uint64_t bits::get_nbits(uint64_t pos, std::size_t digits)
{
    std::size_t max_num_digits = std::numeric_limits<uint64_t>::digits;
    if (digits > max_num_digits) {
        throw std::invalid_argument("The digits must less than " +
                                    std::to_string(max_num_digits) + ".");
    }

    /* Open start */
    auto p_start = get_num_block(pos);

    /* Closed end */
    auto p_end = pos + digits <= m_len ? get_num_block(pos + digits)
                                       : get_num_block(m_len);

    uint64_t res = 0;

    if (p_start.first == p_end.first) {
        res = (m_bitarr[p_start.first] >> p_start.second) &
              ((1 << (p_end.second - p_start.second)) - 1);
        return res;
    }

    for (std::size_t pos = p_end.first + 1; pos-- > p_start.first;) {
        if (pos == p_start.first) {
            res <<= (block_size - p_start.second);
            res |= (m_bitarr[pos] >> p_start.second);
            /* res |= (m_bitarr[pos] >> p_start.second)
             * & (1 << (block_size - p_start.second)) - 1); */
        } else if (pos == p_end.first) {
            res |= m_bitarr[pos] & ((1 << p_end.second) - 1);
        } else {
            res <<= block_size;
            res |= m_bitarr[pos];
        }
    }
    return res;
}

void bits::set_nbits(uint64_t val, uint64_t pos, std::size_t digits)
{
    std::size_t max_num_digits = std::numeric_limits<uint64_t>::digits;
    if (digits > max_num_digits) {
        throw std::invalid_argument("The digits must less than " +
                                    std::to_string(max_num_digits) + ".");
    }

    /* Open start */
    auto p_start = get_num_block(pos);

    /* Closed end */
    auto p_end = pos + digits <= m_len ? get_num_block(pos + digits)
                                       : get_num_block(m_len);

    /* Ensure val is at most n-bit long */
    val &= ((1ULL << digits) - 1);

    if (p_start.first == p_end.first) {
        Block mask = ((1 << digits) - 1) << p_start.second;
        m_bitarr[p_start.first] &= ~mask;
        m_bitarr[p_start.first] |= static_cast<Block>(val) << p_start.second;
        return;
    }

    for (std::size_t pos = p_start.first; pos <= p_end.first; pos++) {
        if (pos == p_start.first) {
            Block mask = ((1 << (block_size - p_start.second)) - 1)
                         << p_start.second;
            std::size_t b = block_size - p_start.second;
            m_bitarr[pos] &= ~mask;
            m_bitarr[pos] |= static_cast<Block>(val & ((1 << b) - 1))
                             << p_start.second;
            val >>= b;
        } else if (pos == p_end.first) {
            Block mask = (1 << p_end.second) - 1;
            m_bitarr[pos] &= ~mask;
            m_bitarr[pos] |= val & mask;
        } else {
            /* m_bitarr[pos] = (val & (1ULL << block_size) - 1); */
            m_bitarr[pos] = (val & static_cast<Block>(-1));
            val >>= block_size;
        }
    }
}

void bits::trim_last_block()
{
    auto p = get_num_block();
    std::size_t arr_size = get_arr_size();

    if (p.second != 0) {
        m_bitarr[arr_size - 1] &= ((1 << p.second) - 1);
    }
}

bits &bits::operator>>=(uint64_t val)
{
    std::size_t cnt = 0;
    std::size_t arr_size = get_arr_size();

    for (std::size_t pos = val; pos < m_len; pos += block_size) {
        m_bitarr[cnt++] = get_nbits(pos, block_size);
    }
    while (cnt < arr_size) {
        /* TODO: padded with the sign bit */
        m_bitarr[cnt++] = 0;
    }
    return *this;
}

bits &bits::operator<<=(uint64_t val)
{
    /* Fill values from the end to avoid overwriting */
    if (m_len <= val) {
        /* all zeros */
        for (std::size_t i = 0; i < get_arr_size(); i++) {
            m_bitarr[i] = 0;
        }
        return *this;
    }

    std::size_t arr_size = get_arr_size();
    std::size_t pos = arr_size * block_size - val;
    std::size_t arr_pos = arr_size;

    while (true) {
        if (pos >= block_size) {
            pos -= block_size;
            m_bitarr[--arr_pos] = get_nbits(pos, block_size);
        } else {
            /* residue */
            m_bitarr[--arr_pos] = m_bitarr[0] << (block_size - pos);
            break;
        }
    }
    for (std::size_t i = 0; i < arr_pos; i++) {
        m_bitarr[i] = 0;
    }

    trim_last_block();

    return *this;
}


bits &bits::operator+=(const bits &rhs)
{
    return this->append(rhs);
}

bits &bits::operator*=(uint64_t times)
{
    return this->repeat(times);
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
    std::size_t arr_size = b.get_arr_size();

    for (std::size_t i = 0; i < arr_size; i++) {
        b.m_bitarr[i] = ~m_bitarr[i];
    }

    b.trim_last_block();
    return b;
}

bits repl(uint64_t times, bits b)
{
    if (times == 0) {
        throw std::invalid_argument("The times must not be zero");
    }
    return b.repeat(times);
}


bits concat(bits lhs, const bits &rhs)
{
    lhs.append(rhs);
    return lhs;
}

bits operator>>(bits b, uint64_t val)
{
    b >>= val;
    return b;
}

bits operator<<(bits b, uint64_t val)
{
    b <<= val;
    return b;
}


bits operator+(bits lhs, const bits &rhs)
{
    lhs += rhs;
    return lhs;
}

bits operator*(uint64_t times, bits b)
{
    b *= times;
    return b;
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

/*
 *  unsigned integer
 */

auto operator"" _u(unsigned long long val)
{
    return [=](std::size_t len) { return bits{len, val}; };
}

/*
 *  signed integer
 */

auto operator"" _s(unsigned long long val)
{
    return [=](std::size_t len) { return bits{len, val}; };
}


auto operator"" _u(const char *str, std::size_t sz)
{
    return [=]() { return bits{std::string{str, sz}}; };
}


auto operator"" _s(const char *str, std::size_t sz)
{
    return [=]() { return bits{std::string{str, sz}}; };
}


}  // namespace literals

}  // namespace bitslice


#endif  // INCLUDE_BITS_HPP_
