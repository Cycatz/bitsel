#ifndef INCLUDE_BITSLICE_HPP_
#define INCLUDE_BITSLICE_HPP_

#include <algorithm>
#include <bitset>
#include <cstddef>  // for size_t
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>  // for string


class Bits
{
private:
    using Block = uint32_t;
    static constexpr std::size_t block_size = sizeof(Block) * 8;

    std::vector<Block> m_bitarr;
    std::size_t m_len;

    std::pair<std::size_t, std::size_t> get_num_block(std::size_t) const;
    bool check_range(std::size_t s, std::size_t e) const;

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
    /* TODO: define set operation */

    bool operator==(const Bits &) const;
    Bits &operator+=(const Bits &);
    Bits &operator&=(const Bits &);
    Bits &operator|=(const Bits &);
    Bits &operator^=(const Bits &);
    Bits operator~() const;
};

std::pair<std::size_t, std::size_t> Bits::get_num_block(std::size_t s) const
{
    return std::make_pair(s / block_size, s % block_size);
}

bool Bits::check_range(std::size_t s, std::size_t e) const
{
    return s < m_len && s >= e;
}


Bits::Bits(std::size_t len) : m_len{len}
{
    if (len == 0) {
        throw std::invalid_argument("The length must not be zero");
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

    auto p = get_num_block(m_len);

    m_bitarr.reserve(p.first + static_cast<std::size_t>(!!p.second));

    for (std::size_t i = 0; i < str.length(); i += block_size) {
        Block curblk = 0;
        for (std::size_t k = std::min(str.length() - i, block_size); k-- > 0;) {
            curblk = curblk << 1 | static_cast<Block>(str[i + k] == '1');
        }
        m_bitarr.push_back(curblk);
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

    for (size_t i = 0; i < m_bitarr.size(); i++) {
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
// Bits &Bits::operator&=(const Bits &rhs)
//{
//    std::size_t pos = 0;
//    std::string b;
//
//    while (true) {
//        if (pos >= m_bitstr.size() && pos >= rhs.m_bitstr.size()) {
//            break;
//        }
//        auto c = [&]() {
//            bool a = false;
//            bool b = false;
//            if (pos < m_bitstr.size()) {
//                a = static_cast<bool>(m_bitstr[pos] - '0');
//            }
//            if (pos < rhs.m_bitstr.size()) {
//                b = static_cast<bool>(rhs.m_bitstr[pos] - '0');
//            }
//            return static_cast<char>(static_cast<char>(a && b) + '0');
//        }();
//        b.push_back(c);
//        pos++;
//    }
//
//    m_bitstr = b;
//    return (*this);
//}
// Bits &Bits::operator|=(const Bits &rhs)
//{
//    std::size_t pos = 0;
//    std::string b;
//
//    while (true) {
//        if (pos >= m_bitstr.size() && pos >= rhs.m_bitstr.size()) {
//            break;
//        }
//        auto c = [&]() {
//            bool a = false;
//            bool b = false;
//            if (pos < m_bitstr.size()) {
//                a = static_cast<bool>(m_bitstr[pos] - '0');
//            }
//            if (pos < rhs.m_bitstr.size()) {
//                b = static_cast<bool>(rhs.m_bitstr[pos] - '0');
//            }
//            return static_cast<char>(static_cast<char>(a || b) + '0');
//        }();
//        b.push_back(c);
//        pos++;
//    }
//
//    m_bitstr = b;
//    return (*this);
//}
// Bits &Bits::operator^=(const Bits &rhs)
//{
//    std::size_t pos = 0;
//    std::string b;
//
//    while (true) {
//        if (pos >= m_bitstr.size() && pos >= rhs.m_bitstr.size()) {
//            break;
//        }
//        auto c = [&]() {
//            bool a = false;
//            bool b = false;
//            if (pos < m_bitstr.size()) {
//                a = static_cast<bool>(m_bitstr[pos] - '0');
//            }
//            if (pos < rhs.m_bitstr.size()) {
//                b = static_cast<bool>(rhs.m_bitstr[pos] - '0');
//            }
//            return static_cast<char>(static_cast<char>(a != b) + '0');
//        }();
//        b.push_back(c);
//        pos++;
//    }
//
//    m_bitstr = b;
//    return (*this);
//}
// Bits Bits::operator~() const
//{
//    std::string b;
//    std::for_each(m_bitstr.begin(), m_bitstr.end(), [&](char c) {
//        if ('0' == c) {
//            b.push_back('1');
//        } else {
//            b.push_back('0');
//        }
//    });
//    return Bits{b};
//}


#endif  // INCLUDE_BITSLICE_HPP_
