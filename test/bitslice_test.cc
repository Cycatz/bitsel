
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


TEST(SliceTestValid, BasicAssertions)
{
    using namespace std::string_literals;

    Bits b1a{3, 0}, b1b{2, 0};
    EXPECT_TRUE(b1a(2, 0) == b1b);
    EXPECT_TRUE(b1a(2, 0).to_string() == "000"s);

    Bits b2a{5, 0}, b2b{2, 0};
    EXPECT_TRUE(b2a(3, 1) == b2b);
    EXPECT_TRUE(b2a(3, 1).to_string() == "000"s);
}


TEST(ANDTest, BasicAssertions)
{
    using namespace std::string_literals;

    Bits a{"01010"};
    Bits b{"01100"};

    a &= b;
    EXPECT_TRUE(a.to_string() == "01000"s);
}

TEST(ORTest, BasicAssertions)
{
    using namespace std::string_literals;

    Bits a{"01010"};
    Bits b{"01100"};

    a |= b;
    EXPECT_TRUE(a.to_string() == "01110"s);
}

TEST(XORTest, BasicAssertions)
{
    using namespace std::string_literals;

    Bits a{"01010"};
    Bits b{"01100"};

    a ^= b;
    EXPECT_TRUE(a.to_string() == "00110"s);
}

TEST(NOTTest, BasicAssertions)
{
    using namespace std::string_literals;

    Bits a{"01010"};
    Bits b{"10101"};

    EXPECT_TRUE(~a == b);
}
