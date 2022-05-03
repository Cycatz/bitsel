#include <iostream>

#include "bitslice.hpp"

int main()
{
    bitslice::bits b1(8, 0xDE);
    bitslice::bits b2(8, 0xBB);
    std::cout << (b1 & b2) << std::endl;
}
