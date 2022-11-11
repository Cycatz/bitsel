
#include <gtest/gtest.h>
#include <gtest/gtest_pred_impl.h>

#include "bitsel.hpp"

#include <array>
#include <limits>

using namespace bitsel;
using namespace bitsel::literals;
using namespace std::string_literals;


TEST(UIntConstructorTest, EmptyTest)
{
    bits b0;
    EXPECT_TRUE(b0.empty());

    bits b0a{0, 0xAA};
    EXPECT_TRUE(b0a.empty());
}

TEST(UIntConstructorTest, BasicTest)
{
    bits b1{8, 0xAA};
    EXPECT_EQ(b1, 0xAA_u(8_w));

    bits b2{10, 0xBC};
    EXPECT_EQ(b2, 0xBC_u(10_w));

    bits b3{64, 0xDEADBEEFDEADBEEF};
    EXPECT_EQ(b3, 0xDEADBEEFDEADBEEF_u(64_w));
}

TEST(UIntConstructorTest, AdvancedTest)
{
    /* The length is smaller the width of the value */
    bits b1{4, 0xAA};
    EXPECT_EQ(b1, 0xA_u(4_w));
}


TEST(BitsBitsStringConstructorTest, EmptyTest)
{
    bits::bitstring bs0{"0xAA"};
    bits b0{0, bs0};
    EXPECT_TRUE(b0.empty());
}

TEST(BitsBitsStringConstructorTest, BasicTest)
{
    bits::bitstring bs1{"0xDEADbeef"};
    bits b1{bs1};
    EXPECT_EQ(b1, "0xDEADBEEF"_u(32_w));

    bits::bitstring bs2{"0o72733"};
    bits b2{20, bs2};
    EXPECT_EQ(b2, "0o72733"_u(20_w));

    bits::bitstring bs3{"0b1101010101"};
    bits b3{30, bs3};
    EXPECT_EQ(b3, "0b1101010101"_u(30_w));
}

TEST(BitsBitsStringConstructorTest, AdvancedTest)
{
    bits::bitstring bs1{"0xDEADbeef"};
    bits b1{10, bs1};
    EXPECT_EQ(b1, "0b1011101111"_u(10_w));
}

TEST(StringConstructorTest, BasicTest)
{
    bits b1{"0b0101"};
    EXPECT_EQ(b1, "0b0101"_u(4_w));
}

TEST(StringConstructorTest, LongWidthTest)
{
    bits b1{"0xABCDEF1234567890CAFE"};
    EXPECT_EQ(b1, "0xABCDEF1234567890CAFE"_u(80_w));
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

TEST(CopyConstructorTest, ZeroLengthTest)
{
    bits b0;
    bits b1(b0);
    EXPECT_EQ(b0, b1);
}

TEST(CopyConstructorTest, BasicTest)
{
    bits b1{"0xDEADBEEF"};
    bits b2(b1);
    EXPECT_EQ(b1, b2);
}

TEST(MoveConstructorTest, ZeroLengthTest)
{
    bits b1;
    bits b2(std::move(b1));

    EXPECT_TRUE(b1.empty());
    EXPECT_TRUE(b2.empty());
}

TEST(MoveConstructorTest, BasicTest)
{
    bits b1{"0xABADBABE"};
    bits b2(std::move(b1));

    EXPECT_EQ(b2, "0xABADBABE"_u(32_w));
}


TEST(CopyAssignmentTest, ZeroLengthTest)
{
    bits b1{"0b010100010101000100001010100010101000100001"};
    bits b2;

    b2 = b1;
    EXPECT_EQ(b1, b2);

    bits b3;
    bits b4{"0b010100010101000100001010100010101000100001"};
    ;

    b4 = b3;
    EXPECT_TRUE(b4.empty());
    EXPECT_EQ(b3, b4);
}

TEST(CopyAssignmentTest, BasicTest)
{
    bits b1{"0b010100010101000100001010100010101000100001"};
    bits b2{"0b010100011110000100001010000110101000100001"};

    b2 = b1;
    EXPECT_EQ(b1, b2);
}

TEST(MoveAssignmentTest, ZeroLengthTest)
{
    bits b1{"0b010100010101000100001010100010101000100001"};
    bits b2;

    b2 = std::move(b1);
    EXPECT_EQ(b2, "0b010100010101000100001010100010101000100001"_u());

    bits b3;
    bits b4{"0b010100010101000100001010100010101000100001"};

    b4 = std::move(b3);
    EXPECT_TRUE(b4.empty());
}

TEST(MoveAssignmentTest, BasicTest)
{
    bits b1{"0b010100010101000100001010100010101000100001"};
    bits b2{"0b010100011111000100001010000010101000100001"};

    b2 = std::move(b1);
    EXPECT_TRUE(b2 == "0b010100010101000100001010100010101000100001"_u());
}


TEST(BitAccessTest, BasicTest)
{
    bits b1{"0b010100010101000100001010100010101000100001"};
    EXPECT_TRUE(b1[0]);
    EXPECT_FALSE(b1[7]);
    EXPECT_TRUE(b1[38]);
    EXPECT_FALSE(b1[41]);
}

TEST(BitSetTest, BasicTest)
{
    bits b_base = "0xDEADBEEF"_u(32_w);
    bits b_test = b_base;

    // 0 -> 0
    ASSERT_FALSE(b_test[29]);
    b_test.set(29, false);
    ASSERT_EQ(b_test, b_base);
    b_test = b_base;

    // 0 -> 1
    ASSERT_FALSE(b_test[24]);
    b_test.set(24, true);
    ASSERT_EQ(b_test, b_base ^ bits(32, 1 << 24));
    b_test = b_base;

    // 1 -> 0
    ASSERT_TRUE(b_test[21]);
    b_test.set(21, false);
    ASSERT_EQ(b_test, b_base ^ bits(32, 1 << 21));
    b_test = b_base;

    // 1 -> 1
    ASSERT_TRUE(b_test[19]);
    b_test.set(19, true);
    ASSERT_EQ(b_test, b_base);
    b_test = b_base;
}

TEST(InitializerListConstructorTest, ZeroLengthTest)
{
    bits b_empty({});

    bits b1, b2, b3, b4;
    bits b_4_zero_vars = {b1, b2, b3, b4};

    EXPECT_TRUE(b_4_zero_vars.empty());
}

TEST(InitializerListConstructorTest, BasicTest)
{
    bits b1{"0b0101"};
    bits b2{"0b1101"};

    bits b3 = {b1, b2};
    EXPECT_TRUE(b3 == "0b01011101"_u(8_w));

    bits b4{"0xABADBABE"};
    bits b5{"0xDEADBEEF"};

    bits b6 = {b4, b5};
    EXPECT_EQ(b6, "0xABADBABEDEADBEEF"_u(64_w));
}


TEST(InitializerListConstructorTest, AdvancedTest)
{
    bits b1 = "0xDEADE"_u(20_w);
    bits b2 = "0xAAAAAA"_u(24_w);
    bits b3 = "0o124411"_u(18_w);
    bits b4 = "0b01010101000010"_u(14_w);

    bits b = {b1, b2, b3, b4};
    EXPECT_EQ(b, "0xDEADEAAAAAA2A425542"_u(76_w));
}

TEST(ToStringTestValid, EmptyTest)
{
    bits b0;
    EXPECT_EQ(b0.to_string(num_base::bin), "");
    EXPECT_EQ(b0.to_string(num_base::hex), "");
    EXPECT_EQ(b0.to_string(num_base::oct), "");
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

    EXPECT_EQ(b1a(2, 0), "0b000"_u());
    EXPECT_EQ(b2a(3, 1), "0b100"_u());
}

TEST(SliceTestValid, AdvancedTest)
{
    bits b2a{"0b101010110101010010001001010101000000010101010000001000"};
    bits b2b{"0b10101001000100101010100000001010"};
    EXPECT_EQ(b2a(44, 13), b2b);
}

TEST(SliceTestInValid, BasicTest)
{
    EXPECT_THROW(
        {
            try {
                bits b0;
                bits t0 = b0(0, 0);
            } catch (const std::invalid_argument &e) {
                EXPECT_STREQ("range error", e.what());
                throw;
            }
        },
        std::out_of_range);

    EXPECT_THROW(
        {
            try {
                bits b1;
                bits t1 = b1(0, 1);
            } catch (const std::invalid_argument &e) {
                EXPECT_STREQ("range error", e.what());
                throw;
            }
        },
        std::out_of_range);

    EXPECT_THROW(
        {
            try {
                bits b2 = bits::zeros(4);
                bits t2 = b2(0, 3);
            } catch (const std::invalid_argument &e) {
                EXPECT_STREQ("range error", e.what());
                throw;
            }
        },
        std::out_of_range);

    EXPECT_THROW(
        {
            try {
                bits b3 = bits::zeros(4);
                bits t3 = b3(b3.width(), 0);
            } catch (const std::invalid_argument &e) {
                EXPECT_STREQ("range error", e.what());
                throw;
            }
        },
        std::out_of_range);
}

TEST(GetNBitsTest, ZeroLengthTest)
{
    bits b;
    auto i = b.get_nbits(0, 6);
    EXPECT_TRUE(i == 0);

    auto j = b.get_nbits(100, 10);
    EXPECT_TRUE(j == 0);
}

TEST(GetNBitsTest, BasicTest)
{
    bits b = 0xDEADBEEF_u(32_w);
    auto i = b.get_nbits(0, 6);
    EXPECT_TRUE(i == 0x2f);

    bits c = 0xDEADBEEFDEADBEEF_u(64_w);
    i = c.get_nbits(20, 32);
    EXPECT_TRUE(i == 0xDBEEFDEA);
}

TEST(SetNBitsTest, ZeroLengthTest)
{
    bits b;
    b.set_nbits(0x1E, 0, 6);
    b.set_nbits(0xFEEDBABE, 20, 32);

    EXPECT_TRUE(b.empty());
}

TEST(SetNBitsTest, BasicTest)
{
    bits b = 0xDEADBEEF_u(32_w);
    b.set_nbits(0x1E, 0, 6);
    EXPECT_TRUE(b == 0xDEADBEDE_u(32_w));

    bits c = 0xDEADBEEFDEADBEEF_u(64_w);
    c.set_nbits(0xFEEDBABE, 20, 32);
    EXPECT_TRUE(c == 0xDEAFEEDBABEDBEEF_u(64_w));
}

TEST(EmptyTest, BasicTest)
{
    bits b;
    EXPECT_TRUE(b.empty());
    EXPECT_EQ(b, bits{});
}

TEST(ReverseTest, BasicTest)
{
    bits a{"0b01011"};
    bits b = a.reverse();

    EXPECT_EQ(b, "0b11010"_u());


    bits c{"0xAABBCCDDEEFF"};
    bits d = c.reverse();

    EXPECT_EQ(d, "0xFF77BB33DD55"_u());
}

TEST(RepeatTest, ZeroLengthTest)
{
    bits a;
    a.repeat(3);
    EXPECT_TRUE(a.empty());

    bits b;
    b.repeat(10000);
    EXPECT_TRUE(b.empty());

    bits c;
    c.repeat(0);
    EXPECT_TRUE(c.empty());
}

TEST(RepeatTest, ZeroTimeTest)
{
    bits a{"0b01010"};
    a.repeat(0);

    EXPECT_TRUE(a.empty());

    bits b{"0xDEADBEEF"};
    b.repeat(0);

    EXPECT_TRUE(b.empty());
}

TEST(RepeatTest, BasicTest)
{
    bits a{"0b01010"};

    a.repeat(3);
    EXPECT_EQ(a, "0b010100101001010"_u());
}

TEST(AppendTest, ZeroLengthTest)
{
    bits a;
    bits b{"0b01100"};

    a.append(b);
    EXPECT_EQ(a, b);

    bits c{"0b01100"};
    bits d;
    c.append(d);
    EXPECT_EQ(c, "0b01100"_u());


    bits e;
    bits f;
    e.append(f);
    EXPECT_TRUE(e.empty());
}

TEST(AppendTest, BasicTest)
{
    bits a{"0b01010"};
    bits b{"0b01100"};

    a.append(b);
    EXPECT_EQ(a, "0b0110001010"_u());
}

TEST(AppendTest, AdvancedTest)
{
    bits a{"0xBADBABE"};
    bits b{"0xEBABDAB"};

    a.append(b);
    EXPECT_EQ(a, "0xEBABDABBADBABE"_u());
}

TEST(FillTest, ZeroLengthTest)
{
    bits a;
    bits b = fill(3, a);
    EXPECT_TRUE(b.empty());

    bits c;
    bits d = fill(10000, b);
    EXPECT_TRUE(d.empty());

    bits e;
    bits f = fill(0, a);
    EXPECT_TRUE(f.empty());
}

TEST(FillTest, ZeroTimeTest)
{
    bits a{"0b01010"};
    bits b = fill(0, a);
    EXPECT_TRUE(b.empty());

    bits c{"0xDEADBEEF"};
    bits d = fill(0, b);
    EXPECT_TRUE(d.empty());
}

TEST(FillTest, BasicTest)
{
    bits a{"0b011"};
    bits b = fill(2, fill(3, a));
    EXPECT_EQ(b, "0b011011011011011011"_u());

    bits c{"0xDEADC"};
    bits d = fill(3, fill(1, c));
    EXPECT_EQ(d, "0xDEADCDEADCDEADC"_u());
}

TEST(CatTest, BasicTest)
{
    bits a{"0b01010"};
    bits b{"0b01100"};
    bits c{"0b11001"};

    bits d = cat(a, cat(b, c));
    EXPECT_EQ(d, "0b010100110011001"_u());

    bits e{28, 0xEADBEEF};
    bits f{40, 0xDDDEADBEEF};
    bits g = cat(e, f);
    EXPECT_EQ(g, "0xEADBEEFDDDEADBEEF"_u());
}

TEST(RightShiftTest, ZeroLengthTest)
{
    bits b;
    bits t;

    t = b >> 0;
    EXPECT_TRUE(t.empty());

    t = b >> 10;
    EXPECT_TRUE(t.empty());

    t = b >> 1000;
    EXPECT_TRUE(t.empty());

    t = b >> 10000;
    EXPECT_TRUE(t.empty());
}

TEST(RightShiftTest, BasicTest)
{
    /* TODO: Need more robust tests */

    bits a{64, 0xDEADBEEF12345678};
    bits b = bits::zeros(64);

    b = a >> 0;
    a >>= 0;
    EXPECT_TRUE(a == 0xDEADBEEF12345678_u(64_w));
    EXPECT_TRUE(b == 0xDEADBEEF12345678_u(64_w));

    b = a >> 4;
    a >>= 4;
    EXPECT_TRUE(a == 0x0DEADBEEF1234567_u(64_w));
    EXPECT_TRUE(b == 0x0DEADBEEF1234567_u(64_w));

    b = a >> 1;
    a >>= 1;
    EXPECT_TRUE(a == 0x06F56DF77891A2B3_u(64_w));
    EXPECT_TRUE(b == 0x06F56DF77891A2B3_u(64_w));

    b = a >> 7;
    a >>= 7;
    EXPECT_TRUE(a == 0x000DEADBEEF12345_u(64_w));
    EXPECT_TRUE(b == 0x000DEADBEEF12345_u(64_w));

    b = a >> 20;
    a >>= 20;
    EXPECT_TRUE(a == 0x00000000DEADBEEF_u(64_w));
    EXPECT_TRUE(b == 0x00000000DEADBEEF_u(64_w));

    b = a >> 100000;
    a >>= 100000;
    EXPECT_TRUE(a == 0_u(64_w));
    EXPECT_TRUE(b == 0_u(64_w));
}

TEST(LeftShiftTest, ZeroLengthTest)
{
    bits b;
    bits t;

    t = b << 0;
    EXPECT_TRUE(t.empty());

    t = b << 10;
    EXPECT_TRUE(t.empty());

    t = b << 1000;
    EXPECT_TRUE(t.empty());

    t = b << 10000;
    EXPECT_TRUE(t.empty());
}

TEST(LeftShiftTest, TwoBlockTest)
{
    /* TODO: Need more robust tests */

    bits a{36, 0x1DEADBEEF};
    bits b = bits::zeros(36);

    b = a << 0;
    a <<= 0;
    EXPECT_TRUE(a == 0x1DEADBEEF_u(36_w));
    EXPECT_TRUE(b == 0x1DEADBEEF_u(36_w));

    b = a << 4;
    a <<= 4;
    EXPECT_TRUE(a == 0xDEADBEEF0_u(36_w));
    EXPECT_TRUE(b == 0xDEADBEEF0_u(36_w));

    b = a << 3;
    a <<= 3;
    EXPECT_TRUE(a == 0xF56DF7780_u(36_w));
    EXPECT_TRUE(b == 0xF56DF7780_u(36_w));

    b = a << 9;
    a <<= 9;
    EXPECT_TRUE(a == 0xDBEEF0000_u(36_w));
    EXPECT_TRUE(b == 0xDBEEF0000_u(36_w));
}

TEST(LeftShiftTest, SingleBlockTest)
{
    bits a{16, 0xDEAD};
    bits b = bits::zeros(16);

    b = a << 8;
    a <<= 8;
    EXPECT_TRUE(a == 0xAD00_u(16_w));
    EXPECT_TRUE(b == 0xAD00_u(16_w));
}

TEST(LeftShiftTest, SingleFullBlockTest)
{
    bits a{32, 0x12345678};
    bits b = bits::zeros(32);

    b = a << 8;
    a <<= 8;
    EXPECT_TRUE(a == 0x34567800_u(32_w));
}

TEST(LeftShiftTest, LargeShiftTest)
{
    bits a{8, 0b10010101};
    bits b = bits::zeros(8);

    b = a << 100000;
    a <<= 100000;
    EXPECT_TRUE(a == 0b00000000_u(8_w));
    EXPECT_TRUE(b == 0b00000000_u(8_w));
}

TEST(ANDTest, ZeroLengthTest)
{
    bits a;
    bits b;
    bits c;

    c = a & b;
    EXPECT_TRUE(c.empty());

    a = 0xDEADBEEF_u();
    b &= a;
    EXPECT_EQ(b, 0_u(bitwidth{a.width()}));
}

TEST(ANDTest, BasicTest)
{
    bits a{"0xDEADBEEF224"};
    bits b{"0xABADBABE828"};
    bits c{"0o1222"};

    a &= b;
    EXPECT_EQ(a, "0x8AADBAAE020"_u(44_w));

    bits d = a & c;
    EXPECT_EQ(d, "0x0"_u(44_w));
}

TEST(ORTest, ZeroLengthTest)
{
    bits a;
    bits b;
    bits c;

    c = a | b;
    EXPECT_TRUE(c.empty());

    a = 0xDEADBEEF_u();
    b |= a;
    EXPECT_EQ(b, 0xDEADBEEF_u());
}

TEST(ORTest, BasicTest)
{
    bits a{"0xDEADBEEF224"};
    bits b{"0xABADBABE828"};
    bits c{"0o1222"};

    a |= b;
    EXPECT_EQ(a, "0xFFADBEFFA2C"_u(44_w));

    bits d = a | c;
    EXPECT_EQ(d, "0xFFADBEFFABE"_u(44_w));
}

TEST(XORTest, ZeroLengthTest)
{
    bits a;
    bits b;
    bits c;

    c = a ^ b;
    EXPECT_TRUE(c.empty());

    a = 0xDEADBEEF_u();
    b ^= a;
    EXPECT_EQ(b, 0xDEADBEEF_u());
}

TEST(XORTest, BasicTest)
{
    bits a{"0xDEADBEEF224"};
    bits b{"0xABADBABE828"};
    bits c{"0o1222"};

    a ^= b;
    EXPECT_EQ(a, "0x75000451A0C"_u(44_w));

    bits d = a ^ c;
    EXPECT_EQ(d, "0x7500045189E"_u(44_w));
}

TEST(PlusTest, ZeroLengthTest)
{
    bits a;
    bits b;
    bits c;

    c = a + b;
    EXPECT_TRUE(c.empty());

    a = 0xDEADBEEF_u();
    b += a;
    EXPECT_EQ(b, 0xDEADBEEF_u());
}

TEST(PlusTest, BasicTest)
{
    /* TODO: Need more robust tests */

    bits a{"0xDEADBEEF224"};
    bits b{"0xABADBABE828"};
    bits c{"0o1222"};

    a += b;
    EXPECT_EQ(a, "0x8A5B79ADA4C"_u(44_w));

    bits d = a + c;
    EXPECT_EQ(d, "0x8A5B79ADCDE"_u(44_w));
}

TEST(SubtractTest, ZeroLengthTest)
{
    bits a;
    bits b;
    bits c;

    c = a - b;
    EXPECT_TRUE(c.empty());

    a = 0xDEADBEEF_u();
    b -= a;
    EXPECT_EQ(b, 0x21524111_u(bitwidth(a.width())));
}

TEST(SubtractTest, BasicTest)
{
    /* TODO: Need more robust tests */

    bits a{"0xDEADBEEF224"};
    bits b{"0xABADBABE828"};
    bits c{"0o1222"};

    a -= b;
    EXPECT_EQ(a, "0x330004309FC"_u(44_w));

    a -= b;
    EXPECT_EQ(a, "0x875249721d4"_u(44_w));
}

TEST(NOTTest, ZeroLengthTest)
{
    bits b;
    EXPECT_EQ(b, ~b);
}

TEST(NOTTest, BasicTest)
{
    bits a{"0xDEADBEEF1234567890"};
    bits b{"0x21524110edcba9876f"};

    EXPECT_EQ(~a, b);
}


TEST(bitstringConstructor, BasicStringTest)
{
    bits::bitstring bs0{"0x"};
    bits::bitstring bs1{"0xDeadbEef"};
    bits::bitstring bs2{"0b10101011"};
    bits::bitstring bs3{"0o73737373"};

    EXPECT_TRUE(bs0.get_base() == num_base::hex);
    EXPECT_TRUE(bs0.get_bitstr() == "");
    EXPECT_TRUE(bs0.get_width() == 0);

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

    std::array<uint32_t, 8> testcases = {
        0,                   // Zero
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

    std::array<uint32_t, 8> testcases = {
        0,                   // Zero
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

    std::array<uint32_t, 6> testcases = {
        0,        // Zero
        2, 3, 7,  // Normal test cases
        8, 10     // Out-of-bound test cases
    };

    for (auto nbits : testcases) {
        uint64_t res = bs.get_nbits(0, nbits);
        EXPECT_EQ(res, 0b11011100 & ((1ULL << std::min(32U, nbits)) - 1));
    }
}

TEST(GetNBitsTEST, NullPtrTest)
{
    using namespace bitsel::utils;
    uint64_t res = get_nbits<uint32_t>(nullptr, 8, 32, 0, 32);
    EXPECT_EQ(res, 0);
}

TEST(GetNBitsTEST, BasicTest)
{
    /* TODO: Need more robust tests */
    /* E.g., blkdg = 0, len = 0, ... */

    using namespace bitsel::utils;
    std::array<uint32_t, 4> arr = {0xde, 0xad, 0xbe, 0xef};
    std::reverse(arr.begin(), arr.end());

    uint32_t *arr_ptr = static_cast<uint32_t *>(arr.data());

    std::array<uint32_t, 8> testcases = {
        0,                   // Zero
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
    using namespace bitsel::utils;
    bits b = 0xDEADBEEF_u(32_w);
    uint32_t val = b.to_uint64();

    EXPECT_EQ(val, 0xDEADBEEF);
}

TEST(CountTest, BasicTest)
{
    bits b1 = bits::zeros(16);
    EXPECT_EQ(b1.count(), 0);

    bits b2 = bits::ones(16);
    EXPECT_EQ(b2.count(), 16);

    bits b3 = "0xDEADBEEF"_u(32_w);
    EXPECT_EQ(b3.count(), 24);
}
