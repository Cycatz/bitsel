
#include <gtest/gtest.h>
#include <gtest/gtest_pred_impl.h>

#include "bitslice.hpp"

using namespace bitslice;
using namespace bitslice::literals;
using namespace std::string_literals;

TEST(UIntConstructorTest, BasicTest)
{
    bits b1{8, 0xAA};
    EXPECT_TRUE(b1.to_string() == "10101010");
}



TEST(StringConstructorTest, BasicTest)
{
    bits b1{"0101"};
    EXPECT_TRUE(b1.to_string() == "0101");
}

TEST(StringConstructorTest, LongStringTest)
{
    bits b1{"010100010101000100001010100010101000100001"};
    EXPECT_TRUE(b1.to_string() == "010100010101000100001010100010101000100001");
}

TEST(StringConstructorTest, ZeroLengthTest)
{
    EXPECT_THROW(
        {
            try {
                bits b{""};
            } catch (const std::invalid_argument &e) {
                EXPECT_STREQ("The length must not be zero", e.what());
                throw;
            }
        },
        std::invalid_argument);
}


TEST(CopyConstructorTest, BasicTest)
{
    bits b1{"010100010101000100001010100010101000100001"};
    bits b2(b1);
    EXPECT_TRUE(b1 == b2);
}


TEST(MoveConstructorTest, BasicTest)
{
    bits b1{"010100010101000100001010100010101000100001"};
    bits b2(std::move(b1));
    EXPECT_TRUE(b2.to_string() == "010100010101000100001010100010101000100001");
}


TEST(CopyAssignmentTest, BasicTest)
{
    bits b1{"010100010101000100001010100010101000100001"};
    bits b2{"01010001111l000100001010000010101000100001"};

    b2 = b1;
    EXPECT_TRUE(b1 == b2);
}

TEST(MoveAssignmentTest, BasicTest)
{
    bits b1{"010100010101000100001010100010101000100001"};
    bits b2{"01010001111l000100001010000010101000100001"};

    b2 = std::move(b1);
    EXPECT_TRUE(b2.to_string() == "010100010101000100001010100010101000100001");
}


TEST(BitAccessTest, BasicTest)
{
    bits b1{"010100010101000100001010100010101000100001"};
    EXPECT_TRUE(b1[0]);
    EXPECT_FALSE(b1[7]);
    EXPECT_TRUE(b1[38]);
    EXPECT_FALSE(b1[41]);
}

TEST(InitializerListConstructorTest, BasicTest)
{
    bits b1{"0101"};
    bits b2{"1101"};

    bits b3 = {b1, b2};
    EXPECT_TRUE(b3.to_string() == "01011101"s);
}


TEST(InitializerListConstructorTest, AdvancedTest)
{
    bits b1{"0101101010101010101010101010101010000000001001"};
    bits b2{"1101101010101010101010101010101010001010101101"};
    bits b3{"1101111111111111111111111111111100000000000000"};
    bits b4{"1101111111111111111100000000000000011111111110"};

    bits b = {b1, b2, b3, b4};
    EXPECT_TRUE(b.to_string() ==
                "0101101010101010101010101010101010000000001001"
                "1101101010101010101010101010101010001010101101"
                "1101111111111111111111111111111100000000000000"
                "1101111111111111111100000000000000011111111110"s);
}

TEST(ToStringTestValid, ZeroArgument)
{
    bits b1{"01000101010101000001001010101010010101001010"};
    EXPECT_TRUE(b1.to_string() ==
                "01000101010101000001001010101010010101001010"s);
}
TEST(ToStringTestValid, OneArgument)
{
    bits b1{"01000101010101000001001010101010010101001010"};
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
    bits b1a{4}, b2a{"010101000"};
    EXPECT_TRUE(b1a(2, 0).to_string() == "000"s);
    EXPECT_TRUE(b2a(3).to_string() == "1000"s);
}

TEST(SliceTestValid, AdvancedTest)
{
    bits b2a{"101010110101010010001001010101000000010101010000001000"};
    bits b2b{"10101001000100101010100000001010"};
    EXPECT_TRUE(b2a(44, 13) == b2b);
}


TEST(RepeatTest, BasicTest)
{
    bits a{"01010"};

    a.repeat(3);
    EXPECT_TRUE(a.to_string() == "010100101001010"s);
}


TEST(AppendTest, BasicTest)
{
    bits a{"01010"};
    bits b{"01100"};

    a.append(b);
    EXPECT_TRUE(a.to_string() == "0101001100"s);
}

TEST(AppendTest, AdvancedTest)
{
    bits a{
        "0101011111111111111111111111111111111111111111111110000000000000100000"
        "00"};
    bits b{"01100"};

    a.append(b);
    EXPECT_TRUE(
        a.to_string() ==
        "01010111111111111111111111111111111111111111111111100000000000001000000001100"s);
}

TEST(ReplicationTest, BasicTest)
{
    bits a{"011"};

    bits b = repl(2, repl(3, a));
    EXPECT_TRUE(b.to_string() == "011011011011011011"s);
}

TEST(ConcatTest, BasicTest)
{
    bits a{"01010"};
    bits b{"01100"};
    bits c{"11001"};

    bits d = concat(a, concat(b, c));
    EXPECT_TRUE(d.to_string() == "010100110011001"s);
}


TEST(RightShiftTest, BasicTest)
{
    bits a{64, 0xDEADBEEF12345678};
    a >>= 4;
    EXPECT_TRUE(a == 0x0DEADBEEF1234567_64);
    a >>= 1;
    EXPECT_TRUE(a == 0x06F56DF77891A2B3_64);
    a >>= 7;
    EXPECT_TRUE(a == 0x000DEADBEEF12345_64);
    a >>= 20;
    EXPECT_TRUE(a == 0x00000000DEADBEEF_64);
    a >>= 100000;
    EXPECT_TRUE(a == 0_64);
}

TEST(LeftShiftTest, TwoBlockTest)
{
    bits a{36, 0x1DEADBEEF};
    a <<= 4;
    EXPECT_TRUE(a == 0xDEADBEEF0_36);
    a <<= 3;
    EXPECT_TRUE(a == 0xF56DF7780_36);
    a <<= 9;
    EXPECT_TRUE(a == 0xDBEEF0000_36);
}

TEST(LeftShiftTest, SingleBlockTest)
{
    bits a{16, 0xDEAD};
    a <<= 8;
    EXPECT_TRUE(a == 0xAD00_16);
}

TEST(LeftShiftTest, SingleFullBlockTest)
{
    bits a{32, 0x12345678};
    a <<= 8;
    EXPECT_TRUE(a == 0x34567800_32);
}

TEST(LeftShiftTest, LargeShiftTest)
{
    bits a{8, 0b10010101};
    a <<= 100000;
    EXPECT_TRUE(a == 0b00000000_8);
}

TEST(AdditionTest, BasicTest)
{
    bits a{"010"};
    bits b{"110"};
    bits c = a + b;

    EXPECT_TRUE(c.to_string() == "010110"s);
}

TEST(MultiplicationTest, BasicTest)
{
    bits a{"101"};
    bits b = 3 * a;
    bits c = 2 * b;

    EXPECT_TRUE(b.to_string() == "101101101"s);
    EXPECT_TRUE(c.to_string() == "101101101101101101"s);
}


TEST(ANDTest, BasicTest)
{
    bits a{"01010"};
    bits b{"01100"};
    bits c{"00101"};

    a &= b;

    EXPECT_TRUE(a.to_string() == "01000"s);


    bits d = a & c;
    EXPECT_TRUE(d.to_string() == "00000"s);
}

TEST(ORTest, BasicTest)
{
    bits a{"01010"};
    bits b{"01100"};
    bits c{"00101"};

    a |= b;
    EXPECT_TRUE(a.to_string() == "01110"s);

    bits d = a | c;
    EXPECT_TRUE(d.to_string() == "01111"s);
}

TEST(XORTest, BasicTest)
{
    bits a{"01010"};
    bits b{"01100"};
    bits c{"00101"};

    a ^= b;
    EXPECT_TRUE(a.to_string() == "00110"s);

    bits d = a ^ c;
    EXPECT_TRUE(d.to_string() == "00011"s);
}

TEST(NOTTest, BasicTest)
{
    bits a{"01010"};
    bits b{"10101"};

    EXPECT_TRUE(~a == b);
}
