
#include <gtest/gtest.h>

#include "bitslice.hpp"


using namespace std::string_literals;

TEST(UIntConstructorTest, BasicTest)
{
    Bits b1{8, 0xAA};
    EXPECT_TRUE(b1.to_string() == "10101010");
}



TEST(StringConstructorTest, BasicTest)
{
    Bits b1{"0101"};
    EXPECT_TRUE(b1.to_string() == "0101");
}

TEST(StringConstructorTest, LongStringTest)
{
    Bits b1{"010100010101000100001010100010101000100001"};
    EXPECT_TRUE(b1.to_string() == "010100010101000100001010100010101000100001");
}

TEST(StringConstructorTest, ZeroLengthTest)
{
    EXPECT_THROW(
        {
            try {
                Bits b{""};
            } catch (const std::invalid_argument &e) {
                EXPECT_STREQ("The length must not be zero", e.what());
                throw;
            }
        },
        std::invalid_argument);
}


TEST(CopyConstructorTest, BasicTest)
{
    Bits b1{"010100010101000100001010100010101000100001"};
    Bits b2(b1);
    EXPECT_TRUE(b1 == b2);
}


TEST(MoveConstructorTest, BasicTest)
{
    Bits b1{"010100010101000100001010100010101000100001"};
    Bits b2(std::move(b1));
    EXPECT_TRUE(b2.to_string() == "010100010101000100001010100010101000100001");
}


TEST(CopyAssignmentTest, BasicTest)
{
    Bits b1{"010100010101000100001010100010101000100001"};
    Bits b2{"01010001111l000100001010000010101000100001"};

    b2 = b1;
    EXPECT_TRUE(b1 == b2);
}

TEST(MoveAssignmentTest, BasicTest)
{
    Bits b1{"010100010101000100001010100010101000100001"};
    Bits b2{"01010001111l000100001010000010101000100001"};

    b2 = std::move(b1);
    EXPECT_TRUE(b2.to_string() == "010100010101000100001010100010101000100001");
}


TEST(BitAccessTest, BasicTest)
{
    Bits b1{"010100010101000100001010100010101000100001"};
    EXPECT_TRUE(b1[0]);
    EXPECT_FALSE(b1[7]);
    EXPECT_TRUE(b1[38]);
    EXPECT_FALSE(b1[41]);
}

// TEST(InitializerListConstructorTest, BasicTest)
// {
//     Bits b1{"0101"};
//     Bits b2{"1101"};

//     Bits b3 = {b1, b2};
//     EXPECT_TRUE(b3.to_string() == "01011101"s);
// }

TEST(ToStringTestValid, ZeroArgument)
{
    Bits b1{"01000101010101000001001010101010010101001010"};
    EXPECT_TRUE(b1.to_string() ==
                "01000101010101000001001010101010010101001010"s);
}
TEST(ToStringTestValid, OneArgument)
{
    Bits b1{"01000101010101000001001010101010010101001010"};
    EXPECT_TRUE(b1.to_string(1) == "10");
    EXPECT_TRUE(b1.to_string(4) == "01010");
    EXPECT_TRUE(b1.to_string(40) ==
                "00101010101000001001010101010010101001010");
}
TEST(ToStringTestValid, TwoArgument)
{
    // Bits b3{6};
    // EXPECT_TRUE(b3.to_string(3, 2) == "00"s);
}


// TEST(ToStringTestInvalid, BasicTest1)
// {
//     Bits b1{4};
//     EXPECT_TRUE(b1.to_string(5, 4) == ""s);
// }
// TEST(ToStringTestInvalid, BasicTest2)
// {
//     Bits b2{6};
//     EXPECT_TRUE(b2.to_string(3, 4) == ""s);
// }


TEST(SliceTestValid, BasicTest)
{
    Bits b1a{4}, b1b{3};
    EXPECT_TRUE(b1a(2, 0) == b1b);
    EXPECT_TRUE(b1a(2, 0).to_string() == "000"s);
}

TEST(SliceTestValid, AdvancedTest)
{
    Bits b2a{"101010110101010010001001010101000000010101010000001000"};
    Bits b2b{"10101001000100101010100000001010"};
    EXPECT_TRUE(b2a(44, 13) == b2b);
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
