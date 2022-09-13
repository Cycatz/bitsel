#include <iostream>

#include "bitslice.hpp"


using namespace bitslice;
using namespace bitslice::literals;


bits byteswap(const bits &b)
{
    return {b(7, 0), b(15, 8), b(24, 15), b(31, 24)};
}

bits sign_extension(const bits &b)
{
    return Cat(Fill(32, b(31, 31)), b);
}



int main() {}
