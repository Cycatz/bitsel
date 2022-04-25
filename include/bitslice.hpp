#ifndef INCLUDE_BITSLICE_HPP_
#define INCLUDE_BITSLICE_HPP_

#include <algorithm>
#include <bitset>
#include <cstddef>  // for size_t
#include <stdexcept>
#include <string>  // for string

class Bits
{
private:
    std::string m_bitstr;
    std::size_t m_start, m_end;

    constexpr bool check_range(std::size_t, std::size_t) const;

public:
    explicit Bits(std::size_t s, std::size_t e = 0);
    explicit Bits(const std::string &str)
        : m_bitstr{str}, m_start{str.length()}, m_end{0}
    {
    }

    void reverse();
    constexpr std::size_t get_size() const { return m_start - m_end + 1; }
    constexpr std::pair<std::size_t, std::size_t> get_width()
    {
        return std::make_pair(m_start, m_end);
    }


    std::string to_string() const { return to_string(m_start, m_end); }
    std::string to_string(std::size_t s) const { return to_string(s, m_end); }
    std::string to_string(std::size_t, std::size_t) const;

    Bits operator()(std::size_t, std::size_t) const;
    constexpr bool operator[](std::size_t pos) const;
    /* TODO: define set operation */

    Bits &operator&=(const Bits &);
    Bits &operator|=(const Bits &);
    Bits &operator^=(const Bits &);
    Bits operator~();
};


Bits::Bits(std::size_t s, std::size_t e) : m_start{s}, m_end{e}
{
    if (0 != s && 0 != e) {
        throw std::invalid_argument("Either start or end must be zero");
    }
    std::size_t sz = get_size();
    m_bitstr.resize(sz, '0');
}

constexpr bool Bits::check_range(std::size_t s, std::size_t e) const
{
    return s >= e && m_start >= s && m_end <= e;
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

#endif  // INCLUDE_BITSLICE_HPP_
