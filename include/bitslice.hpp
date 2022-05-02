#ifndef INCLUDE_BITSLICE_HPP_
#define INCLUDE_BITSLICE_HPP_

#include <algorithm>
#include <bitset>
#include <cstddef>  // for size_t
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>  // for string


class Bits
{
private:
    using Block = uint32_t;
    static constexpr std::size_t block_size = sizeof(Block) * 8;

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

    Bits &do_operation(const Bits &,
                       const std::function<Block(Block, Block)> &);

public:
    explicit Bits(std::size_t);
    explicit Bits(const std::string &str);
    Bits(std::initializer_list<Bits>);

    void reverse();
    constexpr std::size_t get_size() const { return m_len; }

    std::string to_string() const { return to_string(m_len - 1, 0); }
    std::string to_string(std::size_t s) const { return to_string(s, 0); }
    std::string to_string(std::size_t, std::size_t) const;

    Bits operator()(std::size_t, std::size_t) const;

    bool test(std::size_t pos) const;
    bool operator[](std::size_t pos) const;

    void set(std::size_t pos, bool val) const;

    bool operator==(const Bits &) const;
    Bits &operator+=(const Bits &);
    Bits &operator&=(const Bits &);
    Bits &operator|=(const Bits &);
    Bits &operator^=(const Bits &);
    Bits operator~() const;
};

Bits::Bits(std::size_t len) : m_bitarr(nullptr), m_len{len}
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

// Bits::Bits(std::initializer_list<Bits> l) : m_len(0)
// {
//     for (const auto &b : l) {
//         (*this) += b;
//     }
// }

Bits::Bits(const std::string &str) : m_len{str.length()}
{
    if (m_len == 0) {
        throw std::invalid_argument("The length must not be zero");
    }

    std::size_t arr_size = get_arr_size();

    m_bitarr = std::make_unique<Block[]>(arr_size);
    for (std::size_t i = 0, j = 0; i < str.length(); i += block_size, j++) {
        Block curblk = 0;
        for (std::size_t k = std::min(str.length() - i, block_size); k-- > 0;) {
            curblk = curblk << 1 | static_cast<Block>(str[i + k] == '1');
        }
        m_bitarr[j] = curblk;
    }
}


bool Bits::test(std::size_t pos) const
{
    if (pos >= m_len) {
        throw std::out_of_range("Position is out of range");
    }
    return this->operator[](pos);
}

bool Bits::operator[](std::size_t pos) const
{
    auto p = get_num_block(pos);
    return static_cast<bool>((m_bitarr[p.first] >> p.second) & 1);
}

void Bits::set(std::size_t pos, bool val) const
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

std::string Bits::to_string(std::size_t s, std::size_t e) const
{
    if (!check_range(s, e)) {
        return "";
    }

    std::string bitstr;
    bitstr.reserve(s - e + 1);

    for (size_t i = e; i <= s; i++) {
        bitstr.append(1, operator[](i) ? '1' : '0');
    }
    return bitstr;
}

Bits Bits::operator()(std::size_t s, std::size_t e) const
{
    if (!check_range(s, e)) {
        throw std::out_of_range("range error");
    }

    const std::string b = to_string(s, e);
    return Bits(b);
}

bool Bits::operator==(const Bits &rhs) const
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


Bits &Bits::do_operation(const Bits &rhs,
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

Bits &Bits::operator&=(const Bits &rhs)
{
    return do_operation(rhs, [](Block x, Block y) { return x & y; });
}

Bits &Bits::operator|=(const Bits &rhs)
{
    return do_operation(rhs, [](Block x, Block y) { return x | y; });
}

Bits &Bits::operator^=(const Bits &rhs)
{
    return do_operation(rhs, [](Block x, Block y) { return x ^ y; });
}

Bits Bits::operator~() const
{
    Bits b(m_len);
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


#endif  // INCLUDE_BITSLICE_HPP_
