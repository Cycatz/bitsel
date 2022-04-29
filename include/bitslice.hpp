#ifndef INCLUDE_BITSLICE_HPP_
#define INCLUDE_BITSLICE_HPP_

#include <algorithm>
#include <bitset>
#include <cstddef>  // for size_t
#include <initializer_list>
#include <stdexcept>
#include <string>  // for string


class Bits
{
private:
    std::string m_bitstr;
    std::size_t m_len;


    constexpr bool check_range(std::size_t s, std::size_t e) const;

public:
    explicit Bits(std::size_t);
    explicit Bits(const std::string &str) : m_bitstr{str}, m_len{str.length()}
    {
        /* TODO: handle str.length() == 0 */
    }
    Bits(std::initializer_list<Bits>);

    void reverse();
    constexpr std::size_t get_size() const { return m_len; }

    std::string to_string() const { return to_string(m_len - 1, 0); }
    std::string to_string(std::size_t s) const { return to_string(s, 0); }
    std::string to_string(std::size_t, std::size_t) const;

    Bits operator()(std::size_t, std::size_t) const;
    constexpr bool operator[](std::size_t pos) const;
    /* TODO: define set operation */

    bool operator==(const Bits &) const;
    Bits &operator+=(const Bits &);
    Bits &operator&=(const Bits &);
    Bits &operator|=(const Bits &);
    Bits &operator^=(const Bits &);
    Bits operator~() const;
};


Bits::Bits(std::size_t len) : m_len{len}
{
    if (len == 0) {
        throw std::invalid_argument("The length must not be zero");
    }
    m_bitstr.resize(len, '0');
}

Bits::Bits(std::initializer_list<Bits> l) : m_len(0)
{
    for (const auto &b : l) {
        (*this) += b;
    }
}

constexpr bool Bits::check_range(std::size_t s, std::size_t e) const
{
    return s < m_len && s >= e;
}

void Bits::reverse()
{
    std::reverse(m_bitstr.begin(), m_bitstr.end());
}

std::string Bits::to_string(std::size_t s, std::size_t e) const
{
    if (!check_range(s, e)) {
        return "";
    }

    return m_bitstr.substr(0, s - e + 1);
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
    return m_bitstr == rhs.m_bitstr;
}

Bits &Bits::operator+=(const Bits &rhs)
{
    m_len += rhs.m_len;
    m_bitstr += rhs.m_bitstr;

    return (*this);
}


Bits &Bits::operator&=(const Bits &rhs)
{
    std::size_t pos = 0;
    std::string b;

    while (true) {
        if (pos >= m_bitstr.size() && pos >= rhs.m_bitstr.size()) {
            break;
        }
        auto c = [&]() {
            bool a = false;
            bool b = false;
            if (pos < m_bitstr.size()) {
                a = static_cast<bool>(m_bitstr[pos] - '0');
            }
            if (pos < rhs.m_bitstr.size()) {
                b = static_cast<bool>(rhs.m_bitstr[pos] - '0');
            }
            return static_cast<char>(static_cast<char>(a && b) + '0');
        }();
        b.push_back(c);
        pos++;
    }

    m_bitstr = b;
    return (*this);
}
Bits &Bits::operator|=(const Bits &rhs)
{
    std::size_t pos = 0;
    std::string b;

    while (true) {
        if (pos >= m_bitstr.size() && pos >= rhs.m_bitstr.size()) {
            break;
        }
        auto c = [&]() {
            bool a = false;
            bool b = false;
            if (pos < m_bitstr.size()) {
                a = static_cast<bool>(m_bitstr[pos] - '0');
            }
            if (pos < rhs.m_bitstr.size()) {
                b = static_cast<bool>(rhs.m_bitstr[pos] - '0');
            }
            return static_cast<char>(static_cast<char>(a || b) + '0');
        }();
        b.push_back(c);
        pos++;
    }

    m_bitstr = b;
    return (*this);
}
Bits &Bits::operator^=(const Bits &rhs)
{
    std::size_t pos = 0;
    std::string b;

    while (true) {
        if (pos >= m_bitstr.size() && pos >= rhs.m_bitstr.size()) {
            break;
        }
        auto c = [&]() {
            bool a = false;
            bool b = false;
            if (pos < m_bitstr.size()) {
                a = static_cast<bool>(m_bitstr[pos] - '0');
            }
            if (pos < rhs.m_bitstr.size()) {
                b = static_cast<bool>(rhs.m_bitstr[pos] - '0');
            }
            return static_cast<char>(static_cast<char>(a != b) + '0');
        }();
        b.push_back(c);
        pos++;
    }

    m_bitstr = b;
    return (*this);
}
Bits Bits::operator~() const
{
    std::string b;
    std::for_each(m_bitstr.begin(), m_bitstr.end(), [&](char c) {
        if ('0' == c) {
            b.push_back('1');
        } else {
            b.push_back('0');
        }
    });
    return Bits{b};
}


#endif  // INCLUDE_BITSLICE_HPP_
