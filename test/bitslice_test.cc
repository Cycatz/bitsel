
#include <gtest/gtest.h>
#include <gtest/gtest_pred_impl.h>

#include "bitslice.hpp"

#include <array>
#include <limits>

using namespace bitslice;
using namespace bitslice::literals;
using namespace std::string_literals;

TEST(UIntConstructorTest, BasicTest)
{
    bits b1{8, 0xAA};
    EXPECT_TRUE(b1.to_string() == "10101010");
}


TEST(BitsBitsStringConstructorTest, BasicTest)
{
    bitstring bs1{"0xDEADbeef"};
    bits b1{bs1};
    EXPECT_EQ(b1, 0xDEADBEEF_u(32));

    bitstring bs2{"0o72733"};
    bits b2{bs2};
    EXPECT_EQ(b2, 072733_u(15));

    bitstring bs3{"0b1101010101"};
    bits b3{bs3};
    EXPECT_EQ(b3, 0b1101010101_u(10));
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

TEST(GetNBitsTest, BasicTest)
{
    bits b = 0xDEADBEEF_u(32);
    auto i = b.get_nbits(0, 6);
    EXPECT_TRUE(i == 0x2f);

    bits c = 0xDEADBEEFDEADBEEF_u(64);
    i = c.get_nbits(20, 32);
    EXPECT_TRUE(i == 0xDBEEFDEA);
}

TEST(SetNBitsTest, BasicTest)
{
    bits b = 0xDEADBEEF_u(32);
    b.set_nbits(0x1E, 0, 6);
    EXPECT_TRUE(b == 0xDEADBEDE_u(32));

    bits c = 0xDEADBEEFDEADBEEF_u(64);
    c.set_nbits(0xFEEDBABE, 20, 32);
    EXPECT_TRUE(c == 0xDEAFEEDBABEDBEEF_u(64));
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

    bits e{28, 0xEADBEEF};
    bits f{40, 0xDDDEADBEEF};
    bits g = {f, e};
    EXPECT_TRUE(g(67, 64) == 0xD_u(4));
    EXPECT_TRUE(g(63, 32) == 0xDDEADBEE_u(32));
    EXPECT_TRUE(g(31, 0) == 0xFEADBEEF_u(32));
}


TEST(RightShiftTest, BasicTest)
{
    bits a{64, 0xDEADBEEF12345678};
    a >>= 4;
    EXPECT_TRUE(a == 0x0DEADBEEF1234567_u(64));
    a >>= 1;
    EXPECT_TRUE(a == 0x06F56DF77891A2B3_u(64));
    a >>= 7;
    EXPECT_TRUE(a == 0x000DEADBEEF12345_u(64));
    a >>= 20;
    EXPECT_TRUE(a == 0x00000000DEADBEEF_u(64));
    a >>= 100000;
    EXPECT_TRUE(a == 0_u(64));
}

TEST(LeftShiftTest, TwoBlockTest)
{
    bits a{36, 0x1DEADBEEF};
    a <<= 4;
    EXPECT_TRUE(a == 0xDEADBEEF0_u(36));
    a <<= 3;
    EXPECT_TRUE(a == 0xF56DF7780_u(36));
    a <<= 9;
    EXPECT_TRUE(a == 0xDBEEF0000_u(36));
}

TEST(LeftShiftTest, SingleBlockTest)
{
    bits a{16, 0xDEAD};
    a <<= 8;
    EXPECT_TRUE(a == 0xAD00_u(16));
}

TEST(LeftShiftTest, SingleFullBlockTest)
{
    bits a{32, 0x12345678};
    a <<= 8;
    EXPECT_TRUE(a == 0x34567800_u(32));
}

TEST(LeftShiftTest, LargeShiftTest)
{
    bits a{8, 0b10010101};
    a <<= 100000;
    EXPECT_TRUE(a == 0b00000000_u(8));
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


TEST(bitstringConstructor, BasicStringTest)
{
    bitstring bs1{"0xDeadbEef"};
    bitstring bs2{"0b10101011"};
    bitstring bs3{"0o73737373"};


    EXPECT_TRUE(bs1.get_base() == num_base::hex);
    EXPECT_TRUE(bs1.get_bitstr() == "deadbeef");
    EXPECT_TRUE(bs1.get_width() == 32);

    EXPECT_TRUE(bs2.get_base() == num_base::bin);
    EXPECT_TRUE(bs2.get_bitstr() == "10101011");
    EXPECT_TRUE(bs2.get_width() == 8);

    EXPECT_TRUE(bs3.get_base() == num_base::oct);
    EXPECT_TRUE(bs3.get_bitstr() == "73737373");
    EXPECT_TRUE(bs3.get_width() == 24);
}


TEST(bitstringConstructor, IllegalStringTest)
{
    EXPECT_THROW(
        {
            try {
                bitstring bs{"x123445"};
            } catch (const std::invalid_argument &e) {
                EXPECT_STREQ("Bit string must prefix with 0x, 0o or 0b!",
                             e.what());
                throw;
            }
        },
        std::invalid_argument);

    EXPECT_THROW(
        {
            try {
                bitstring bs{"0kaaaaa"};
            } catch (const std::invalid_argument &e) {
                EXPECT_STREQ("Bit string must prefix with 0x, 0o or 0b!",
                             e.what());
                throw;
            }
        },
        std::invalid_argument);

    EXPECT_THROW(
        {
            try {
                bitstring bs{"0xzzzyyy"};
            } catch (const std::invalid_argument &e) {
                EXPECT_STREQ("Bit string contain illegal characters!",
                             e.what());
                throw;
            }
        },
        std::invalid_argument);

    EXPECT_THROW(
        {
            try {
                bitstring bs{"0b3338984"};
            } catch (const std::invalid_argument &e) {
                EXPECT_STREQ("Bit string contain illegal characters!",
                             e.what());
                throw;
            }
        },
        std::invalid_argument);

    EXPECT_THROW(
        {
            try {
                bitstring bs{"0o999999"};
            } catch (const std::invalid_argument &e) {
                EXPECT_STREQ("Bit string contain illegal characters!",
                             e.what());
                throw;
            }
        },
        std::invalid_argument);
}

TEST(bitstringConstructor, HexGetNBitsTest)
{
    bitstring bs{"0xDEADBEEF"};

    std::array<uint32_t, 7> testcases = {
        8,  16, 17, 21, 24,  // Normal test cases
        32, 36               // Out-of-bound test cases
    };

    for (auto nbits : testcases) {
        uint64_t res = bs.get_nbits(0, nbits);
        EXPECT_EQ(res, 0xDEADBEEF & ((1ULL << std::min(32U, nbits)) - 1));
    }
}

TEST(bitstringConstructor, OctGetNBitsTest)
{
    bitstring bs{"0o1234567"};

    std::array<uint32_t, 7> testcases = {
        3,  11, 14, 18, 20,  // Normal test cases
        21, 40               // Out-of-bound test cases
    };

    for (auto nbits : testcases) {
        uint64_t res = bs.get_nbits(0, nbits);
        EXPECT_EQ(res, 01234567 & ((1ULL << std::min(32U, nbits)) - 1));
    }
}

TEST(bitstringConstructor, BinGetNBitsTest)
{
    bitstring bs{"0b11011100"};

    std::array<uint32_t, 5> testcases = {
        2, 3, 7,  // Normal test cases
        8, 10     // Out-of-bound test cases
    };

    for (auto nbits : testcases) {
        uint64_t res = bs.get_nbits(0, nbits);
        EXPECT_EQ(res, 0b11011100 & ((1ULL << std::min(32U, nbits)) - 1));
    }
}

TEST(GetNBitsTEST, BasicTest)
{
    using namespace bitslice::utils;
    std::array<uint32_t, 4> arr = {0xde, 0xad, 0xbe, 0xef};
    std::reverse(arr.begin(), arr.end());

    uint32_t *arr_ptr = static_cast<uint32_t *>(arr.data());

    std::array<uint32_t, 7> testcases = {
        8,  16, 17, 21, 24,  // Normal test cases
        32, 36               // Out-of-bound test cases
    };

    for (auto nbits : testcases) {
        uint64_t res = get_nbits<uint32_t>(arr_ptr, 8, 32, 0, nbits);
        EXPECT_EQ(res, 0xDEADBEEF & ((1ULL << std::min(32U, nbits)) - 1));
    }
}
