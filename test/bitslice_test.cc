
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
    EXPECT_EQ(b1, 0xAA_U(8_W));

    bits b2{10, 0xBC};
    EXPECT_EQ(b2, 0xBC_U(10_W));

    bits b3{64, 0xDEADBEEFDEADBEEF};
    EXPECT_EQ(b3, 0xDEADBEEFDEADBEEF_U(64_W));
}

TEST(UIntConstructorTest, AdvancedTest)
{
    /* The length is smaller the width of the value */
    bits b1{4, 0xAA};
    EXPECT_EQ(b1, 0xA_U(4_W));
}


TEST(BitsBitsStringConstructorTest, BasicTest)
{
    bits::bitstring bs1{"0xDEADbeef"};
    bits b1{bs1};
    EXPECT_EQ(b1, "0xDEADBEEF"_U(32_W));

    bits::bitstring bs2{"0o72733"};
    bits b2{20, bs2};
    EXPECT_EQ(b2, "0o72733"_U(20_W));

    bits::bitstring bs3{"0b1101010101"};
    bits b3{30, bs3};
    EXPECT_EQ(b3, "0b1101010101"_U(30_W));
}

TEST(BitsBitsStringConstructorTest, AdvancedTest)
{
    bits::bitstring bs1{"0xDEADbeef"};
    bits b1{10, bs1};
    EXPECT_EQ(b1, "0b1011101111"_U(10_W));
}

TEST(StringConstructorTest, BasicTest)
{
    bits b1{"0b0101"};
    EXPECT_EQ(b1, "0b0101"_U(4_W));
}

TEST(StringConstructorTest, LongWidthTest)
{
    bits b1{"0xABCDEF1234567890CAFE"};
    EXPECT_EQ(b1, "0xABCDEF1234567890CAFE"_U(80_W));
}

TEST(StringConstructorTest, ZeroLengthTest)
{
    EXPECT_THROW(
        {
            try {
                bits b{""};
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
                bits b{"01010101"};
            } catch (const std::invalid_argument &e) {
                EXPECT_STREQ("Bit string must prefix with 0x, 0o or 0b!",
                             e.what());
                throw;
            }
        },
        std::invalid_argument);
}


TEST(CopyConstructorTest, BasicTest)
{
    bits b1{"0xDEADBEEF"};
    bits b2(b1);
    EXPECT_EQ(b1, b2);
}


TEST(MoveConstructorTest, BasicTest)
{
    bits b1{"0xABADBABE"};
    bits b2(std::move(b1));

    EXPECT_EQ(b2, "0xABADBABE"_U(32_W));
}


TEST(CopyAssignmentTest, BasicTest)
{
    bits b1{"0b010100010101000100001010100010101000100001"};
    bits b2{"0b010100011110000100001010000110101000100001"};

    b2 = b1;
    EXPECT_EQ(b1, b2);
}

TEST(MoveAssignmentTest, BasicTest)
{
    bits b1{"0b010100010101000100001010100010101000100001"};
    bits b2{"0b010100011111000100001010000010101000100001"};

    b2 = std::move(b1);
    EXPECT_TRUE(b2 == "0b010100010101000100001010100010101000100001"_U());
}


TEST(BitAccessTest, BasicTest)
{
    bits b1{"0b010100010101000100001010100010101000100001"};
    EXPECT_TRUE(b1[0]);
    EXPECT_FALSE(b1[7]);
    EXPECT_TRUE(b1[38]);
    EXPECT_FALSE(b1[41]);
}

TEST(InitializerListConstructorTest, BasicTest)
{
    bits b1{"0b0101"};
    bits b2{"0b1101"};

    bits b3 = {b1, b2};
    EXPECT_EQ(b3, "0b01011101"_U(8_W));

    bits b4{"0xABADBABE"};
    bits b5{"0xDEADBEEF"};

    bits b6 = {b4, b5};
    EXPECT_EQ(b6, "0xABADBABEDEADBEEF"_U(64_W));
}


TEST(InitializerListConstructorTest, AdvancedTest)
{
    bits b1 = "0xDEADE"_U(20_W);
    bits b2 = "0xAAAAAA"_U(24_W);
    bits b3 = "0o124411"_U(18_W);
    bits b4 = "0b01010101000010"_U(14_W);

    bits b = {b1, b2, b3, b4};
    EXPECT_EQ(b, "0xDEADEAAAAAA2A425542"_U(76_W));
}

TEST(ToStringTestValid, ZeroArgument)
{
    bits b1{"0b01000101010101000001001010101010010101001010"};
    EXPECT_EQ(b1.to_string(num_base::bin),
              "01000101010101000001001010101010010101001010");

    bits b2{"0xDEADBEEFBABAAABBABFABA"};
    EXPECT_EQ(b2.to_string(num_base::hex), "DEADBEEFBABAAABBABFABA");

    bits b3{"0o71272561712462471271712460124"};
    EXPECT_EQ(b3.to_string(num_base::oct), "71272561712462471271712460124");
}
TEST(SliceThenToStringTestValid, OneArgument)
{
    bits b1{"0b01000101010101000001001010101010010101001010"};
    EXPECT_EQ(b1(1, 0).to_string(num_base::bin), "10");
    EXPECT_EQ(b1(3, 0).to_string(num_base::hex), "A");
    EXPECT_EQ(b1(4, 0).to_string(num_base::bin), "01010");
    EXPECT_EQ(b1(7, 0).to_string(num_base::hex), "4A");
    EXPECT_EQ(b1(40, 0).to_string(num_base::bin),
              "00101010101000001001010101010010101001010");


    bits b2{"0xDEADBEEFBABAAABBABFABA"};
    EXPECT_EQ(b2(0, 0).to_string(num_base::hex), "0");
    EXPECT_EQ(b2(1, 0).to_string(num_base::hex), "2");
    EXPECT_EQ(b2(1, 0).to_string(num_base::bin), "10");
    EXPECT_EQ(b2(15, 0).to_string(num_base::hex), "FABA");
    EXPECT_EQ(b2(15, 0).to_string(num_base::oct), "175272");
    EXPECT_EQ(b2(16, 0).to_string(num_base::hex), "1FABA");
    EXPECT_EQ(b2(79, 0).to_string(num_base::hex), "ADBEEFBABAAABBABFABA");
}
TEST(SliceThenToStringTestValid, TwoArgument)
{
    bits b1{"0b01000101010101000001001010101010010101001010"};
    EXPECT_EQ(b1(0, 0).to_string(num_base::bin), "0");
    EXPECT_EQ(b1(4, 1).to_string(num_base::hex), "5");
    EXPECT_EQ(b1(4, 1).to_string(num_base::bin), "0101");
    EXPECT_EQ(b1(7, 3).to_string(num_base::hex), "09");
    EXPECT_EQ(b1(16, 9).to_string(num_base::hex), "52");
    EXPECT_EQ(b1(20, 15).to_string(num_base::oct), "25");
    EXPECT_EQ(b1(40, 22).to_string(num_base::bin), "0010101010100000100");
}

TEST(SliceTestValid, BasicTest)
{
    bits b1a = bits::zeros(4), b2a{"0b010101000"};

    EXPECT_EQ(b1a(2, 0), "0b000"_U());
    EXPECT_EQ(b2a(3, 1), "0b100"_U());
}

TEST(SliceTestValid, AdvancedTest)
{
    bits b2a{"0b101010110101010010001001010101000000010101010000001000"};
    bits b2b{"0b10101001000100101010100000001010"};
    EXPECT_EQ(b2a(44, 13), b2b);
}

TEST(GetNBitsTest, BasicTest)
{
    bits b = 0xDEADBEEF_U(32_W);
    auto i = b.get_nbits(0, 6);
    EXPECT_TRUE(i == 0x2f);

    bits c = 0xDEADBEEFDEADBEEF_U(64_W);
    i = c.get_nbits(20, 32);
    EXPECT_TRUE(i == 0xDBEEFDEA);
}

TEST(SetNBitsTest, BasicTest)
{
    bits b = 0xDEADBEEF_U(32_W);
    b.set_nbits(0x1E, 0, 6);
    EXPECT_TRUE(b == 0xDEADBEDE_U(32_W));

    bits c = 0xDEADBEEFDEADBEEF_U(64_W);
    c.set_nbits(0xFEEDBABE, 20, 32);
    EXPECT_TRUE(c == 0xDEAFEEDBABEDBEEF_U(64_W));
}



TEST(RepeatTest, BasicTest)
{
    bits a{"0b01010"};

    a.repeat(3);
    EXPECT_EQ(a, "0b010100101001010"_U());
}


TEST(AppendTest, BasicTest)
{
    bits a{"0b01010"};
    bits b{"0b01100"};

    a.append(b);
    EXPECT_EQ(a, "0b0101001100"_U());
}

TEST(AppendTest, AdvancedTest)
{
    bits a{"0xBADBABE"};
    bits b{"0xEBABDAB"};

    a.append(b);
    EXPECT_EQ(a, "0xBADBABEEBABDAB"_U());
}

TEST(FillTest, BasicTest)
{
    bits a{"0b011"};
    bits b = Fill(2, Fill(3, a));
    EXPECT_EQ(b, "0b011011011011011011"_U());

    bits c{"0xDEADC"};
    bits d = Fill(3, Fill(1, c));
    EXPECT_EQ(d, "0xDEADCDEADCDEADC"_U());
}

TEST(CatTest, BasicTest)
{
    bits a{"0b01010"};
    bits b{"0b01100"};
    bits c{"0b11001"};

    bits d = Cat(a, Cat(b, c));
    EXPECT_EQ(d, "0b010100110011001"_U());

    bits e{28, 0xEADBEEF};
    bits f{40, 0xDDDEADBEEF};
    bits g = Cat(e, f);
    EXPECT_EQ(g, "0xEADBEEFDDDEADBEEF"_U());
}


TEST(RightShiftTest, BasicTest)
{

    /* TODO: Need more robust tests */

    bits a{64, 0xDEADBEEF12345678};
    bits b = bits::zeros(64);

    b = a >> 0;
    a >>= 0;
    EXPECT_TRUE(a == 0xDEADBEEF12345678_U(64_W));
    EXPECT_TRUE(b == 0xDEADBEEF12345678_U(64_W));

    b = a >> 4;
    a >>= 4;
    EXPECT_TRUE(a == 0x0DEADBEEF1234567_U(64_W));
    EXPECT_TRUE(b == 0x0DEADBEEF1234567_U(64_W));

    b = a >> 1;
    a >>= 1;
    EXPECT_TRUE(a == 0x06F56DF77891A2B3_U(64_W));
    EXPECT_TRUE(b == 0x06F56DF77891A2B3_U(64_W));

    b = a >> 7;
    a >>= 7;
    EXPECT_TRUE(a == 0x000DEADBEEF12345_U(64_W));
    EXPECT_TRUE(b == 0x000DEADBEEF12345_U(64_W));

    b = a >> 20;
    a >>= 20;
    EXPECT_TRUE(a == 0x00000000DEADBEEF_U(64_W));
    EXPECT_TRUE(b == 0x00000000DEADBEEF_U(64_W));

    b = a >> 100000;
    a >>= 100000;
    EXPECT_TRUE(a == 0_U(64_W));
    EXPECT_TRUE(b == 0_U(64_W));
}

TEST(LeftShiftTest, TwoBlockTest)
{

    /* TODO: Need more robust tests */

    bits a{36, 0x1DEADBEEF};
    bits b = bits::zeros(36);

    b = a << 0;
    a <<= 0;
    EXPECT_TRUE(a == 0x1DEADBEEF_U(36_W));
    EXPECT_TRUE(b == 0x1DEADBEEF_U(36_W));

    b = a << 4;
    a <<= 4;
    EXPECT_TRUE(a == 0xDEADBEEF0_U(36_W));
    EXPECT_TRUE(b == 0xDEADBEEF0_U(36_W));

    b = a << 3;
    a <<= 3;
    EXPECT_TRUE(a == 0xF56DF7780_U(36_W));
    EXPECT_TRUE(b == 0xF56DF7780_U(36_W));

    b = a << 9;
    a <<= 9;
    EXPECT_TRUE(a == 0xDBEEF0000_U(36_W));
    EXPECT_TRUE(b == 0xDBEEF0000_U(36_W));
}

TEST(LeftShiftTest, SingleBlockTest)
{
    bits a{16, 0xDEAD};
    bits b = bits::zeros(16);

    b = a << 8;
    a <<= 8;
    EXPECT_TRUE(a == 0xAD00_U(16_W));
    EXPECT_TRUE(b == 0xAD00_U(16_W));
}

TEST(LeftShiftTest, SingleFullBlockTest)
{
    bits a{32, 0x12345678};
    bits b = bits::zeros(32);

    b = a << 8;
    a <<= 8;
    EXPECT_TRUE(a == 0x34567800_U(32_W));
}

TEST(LeftShiftTest, LargeShiftTest)
{
    bits a{8, 0b10010101};
    bits b = bits::zeros(8);

    b = a << 100000;
    a <<= 100000;
    EXPECT_TRUE(a == 0b00000000_U(8_W));
    EXPECT_TRUE(b == 0b00000000_U(8_W));
}


TEST(ANDTest, BasicTest)
{
    bits a{"0xDEADBEEF224"};
    bits b{"0xABADBABE828"};
    bits c{"0o1222"};

    a &= b;
    EXPECT_EQ(a, "0x8AADBAAE020"_U(44_W));

    bits d = a & c;
    EXPECT_EQ(d, "0x0"_U(44_W));
}

TEST(ORTest, BasicTest)
{
    bits a{"0xDEADBEEF224"};
    bits b{"0xABADBABE828"};
    bits c{"0o1222"};

    a |= b;
    EXPECT_EQ(a, "0xFFADBEFFA2C"_U(44_W));

    bits d = a | c;
    EXPECT_EQ(d, "0xFFADBEFFABE"_U(44_W));
}

TEST(XORTest, BasicTest)
{
    bits a{"0xDEADBEEF224"};
    bits b{"0xABADBABE828"};
    bits c{"0o1222"};

    a ^= b;
    EXPECT_EQ(a, "0x75000451A0C"_U(44_W));

    bits d = a ^ c;
    EXPECT_EQ(d, "0x7500045189E"_U(44_W));
}

TEST(PlusTest, BasicTest)
{
    /* TODO: Need more robust tests */

    bits a{"0xDEADBEEF224"};
    bits b{"0xABADBABE828"};
    bits c{"0o1222"};

    a += b;
    EXPECT_EQ(a, "0x8A5B79ADA4C"_U(44_W));

    bits d = a + c;
    EXPECT_EQ(d, "0x8A5B79ADCDE"_U(44_W));
}

TEST(SubtractTest, BasicTest)
{
    /* TODO: Need more robust tests */

    bits a{"0xDEADBEEF224"};
    bits b{"0xABADBABE828"};
    bits c{"0o1222"};

    a -= b;
    EXPECT_EQ(a, "0x330004309FC"_U(44_W));

    a -= b;
    EXPECT_EQ(a, "0x875249721d4"_U(44_W));
}

TEST(NOTTest, BasicTest)
{
    bits a{"0xDEADBEEF1234567890"};
    bits b{"0x21524110edcba9876f"};

    EXPECT_EQ(~a, b);
}


TEST(bitstringConstructor, BasicStringTest)
{
    bits::bitstring bs1{"0xDeadbEef"};
    bits::bitstring bs2{"0b10101011"};
    bits::bitstring bs3{"0o73737373"};


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
                bits::bitstring bs{"x123445"};
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
                bits::bitstring bs{"0kaaaaa"};
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
                bits::bitstring bs{"0xzzzyyy"};
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
                bits::bitstring bs{"0b3338984"};
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
                bits::bitstring bs{"0o999999"};
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
    bits::bitstring bs{"0xDEADBEEF"};

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
    bits::bitstring bs{"0o1234567"};

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
    bits::bitstring bs{"0b11011100"};

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

TEST(ToUInt64Test, BasicTest)
{
    using namespace bitslice::utils;
    bits b = 0xDEADBEEF_U(32_W);
    uint32_t val = b.to_uint64();

    EXPECT_EQ(val, 0xDEADBEEF);
}
