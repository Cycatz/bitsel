#ifndef INCLUDE_BITSLICE_HPP_
#define INCLUDE_BITSLICE_HPP_

#include <cstddef>  // for size_t
#include <string>   // for string

constexpr std::size_t get_size(std::size_t S, std::size_t E)
{
    return 1 + ((S <= E) ? E - S : S - E);
}

template <std::size_t S, std::size_t E>
class Bits
{
private:
    std::string b;

public:
    Bits();
    explicit Bits(const std::string &);

    Bits reverse();

    std::string to_string(std::size_t = S, std::size_t = E) const;

    constexpr bool operator[](std::size_t pos) const;

    /* Slice */
    template <std::size_t T, std::size_t U>
    Bits operator()(std::size_t = T, std::size_t = U) const;

    template <std::size_t T, std::size_t U>
    Bits &operator&=(const Bits<T, U> &);

    template <std::size_t T, std::size_t U>
    Bits &operator|=(const Bits<T, U> &);

    template <std::size_t T, std::size_t U>
    Bits &operator^=(const Bits<T, U> &);

    Bits operator~();
};

#endif  // INCLUDE_BITSLICE_HPP_
