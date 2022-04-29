
#include <gtest/gtest.h>

#include "bitslice.hpp"


using namespace std::string_literals;

TEST(StringConstructorTest, BasicTest)
{
    Bits b1{"0101"};
    EXPECT_TRUE(b1.to_string(3, 0) == "0101"s);
}
TEST(StringConstructorTest, SameIndex)
{
    Bits b2{"1"};
    EXPECT_TRUE(b2.to_string(0, 0) == "1"s);
}



TEST(ToStringTestValid, ZeroArgument)
{
    Bits b1{4};
    EXPECT_TRUE(b1.to_string() == "0000"s);
}
TEST(ToStringTestValid, OneArgument)
{
    Bits b2{2};
    EXPECT_TRUE(b2.to_string(1) == "00"s);
}
TEST(ToStringTestValid, TwoArgument)
{
    Bits b3{6};
    EXPECT_TRUE(b3.to_string(3, 2) == "00"s);
}


TEST(ToStringTestInvalid, BasicTest1)
{
    Bits b1{4};
    EXPECT_TRUE(b1.to_string(5, 4) == ""s);
}
TEST(ToStringTestInvalid, BasicTest2)
{
    Bits b2{6};
    EXPECT_TRUE(b2.to_string(3, 4) == ""s);
}


TEST(SliceTestValid, BasicTest1)
{
    Bits b1a{4}, b1b{3};
    EXPECT_TRUE(b1a(2, 0) == b1b);
    EXPECT_TRUE(b1a(2, 0).to_string() == "000"s);
}
TEST(SliceTestValid, BasicTest2)
{
    Bits b2a{6}, b2b{3};
    EXPECT_TRUE(b2a(3, 1) == b2b);
    EXPECT_TRUE(b2a(3, 1).to_string() == "000"s);
}


TEST(ANDTest, BasicTest)
{
    Bits a{"01010"};
    Bits b{"01100"};

    a &= b;
    EXPECT_TRUE(a.to_string() == "01000"s);
}

TEST(ORTest, BasicTest)
{
    Bits a{"01010"};
    Bits b{"01100"};

    a |= b;
    EXPECT_TRUE(a.to_string() == "01110"s);
}

TEST(XORTest, BasicTest)
{
    Bits a{"01010"};
    Bits b{"01100"};

    a ^= b;
    EXPECT_TRUE(a.to_string() == "00110"s);
}

TEST(NOTTest, BasicTest)
{
    Bits a{"01010"};
    Bits b{"10101"};

    EXPECT_TRUE(~a == b);
}
