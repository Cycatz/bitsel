
#include <gtest/gtest.h>

#include "bitslice.hpp"

TEST(StringConstructorTest, BasicAssertions)
{
    using namespace std::string_literals;

    Bits b1{"0101"};
    EXPECT_TRUE(b1.to_string(3, 0) == "0101"s);


    Bits b2{"1"};
    EXPECT_TRUE(b2.to_string(0, 0) == "1"s);
}
TEST(ToStringTestValid, BasicAssertions)
{
    using namespace std::string_literals;

    Bits b1{3, 0};
    EXPECT_TRUE(b1.to_string() == "0000"s);

    Bits b2{1, 0};
    EXPECT_TRUE(b2.to_string(1) == "00"s);

    Bits b3{5, 0};
    EXPECT_TRUE(b3.to_string(3, 2) == "00"s);
}
TEST(ToStringTestInvalid, BasicAssertions)
{
    using namespace std::string_literals;

    Bits b1{3, 0};
    EXPECT_TRUE(b1.to_string(5, 4) == ""s);

    Bits b2{5, 0};
    EXPECT_TRUE(b1.to_string(3, 4) == ""s);
}
