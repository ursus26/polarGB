#include <gtest/gtest.h>
#include <stdio.h>
#include "polarGB/types.h"
#include "polarGB/register.h"


TEST(RegisterTest, Write8)
{
    Register reg;
    u8 testByte = 0x81;

    /* Register A */
    reg.write8(RegID_A, testByte);
    EXPECT_EQ(reg.read8(RegID_A), testByte);

    /* Register B */
    testByte = 0x15;
    reg.write8(RegID_B, testByte);
    EXPECT_EQ(reg.read8(RegID_B), testByte);

    /* Register C */
    testByte = 0xf4;
    reg.write8(RegID_C, testByte);
    EXPECT_EQ(reg.read8(RegID_C), testByte);

    /* Register D */
    testByte = 0x7f;
    reg.write8(RegID_D, testByte);
    EXPECT_EQ(reg.read8(RegID_D), testByte);

    /* Register E */
    testByte = 0xb6;
    reg.write8(RegID_E, testByte);
    EXPECT_EQ(reg.read8(RegID_E), testByte);

    /* Register H */
    testByte = 0xb8;
    reg.write8(RegID_H, testByte);
    EXPECT_EQ(reg.read8(RegID_H), testByte);

    /* Register L */
    testByte = 0xc8;
    reg.write8(RegID_L, testByte);
    EXPECT_EQ(reg.read8(RegID_L), testByte);
}


TEST(RegisterTest, Write16)
{
    Register reg;

    /* Register AF */
    u16 testNumber = 0x561c;
    reg.write16(RegID_AF, testNumber);
    EXPECT_EQ(reg.read16(RegID_AF), testNumber);
    EXPECT_EQ(reg.read8(RegID_A), testNumber & 0xff00 >> 8);

    /* Register BC */
    testNumber = 0xc074;
    printf("Testbyte: 0x%x, high: 0x%x, low: 0x%x\n", testNumber, (testNumber >> 8) & 0xff, testNumber & 0xff);
    reg.write16(RegID_BC, testNumber);
    EXPECT_EQ(reg.read16(RegID_BC), testNumber);
    EXPECT_EQ(reg.read8(RegID_B), (testNumber >> 8) & 0xff);
    EXPECT_EQ(reg.read8(RegID_C), testNumber & 0xff);

    /* Register DE */
    testNumber = 0xb624;
    printf("Testbyte: 0x%x, high: 0x%x, low: 0x%x\n", testNumber, (testNumber >> 8) & 0xff, testNumber & 0xff);
    reg.write16(RegID_DE, testNumber);
    EXPECT_EQ(reg.read16(RegID_DE), testNumber);
    EXPECT_EQ(reg.read8(RegID_D), (testNumber >> 8) & 0xff);
    EXPECT_EQ(reg.read8(RegID_E), testNumber & 0xff);

    /* Register HL */
    testNumber = 0x46fc;
    printf("Testbyte: 0x%x, high: 0x%x, low: 0x%x\n", testNumber, (testNumber >> 8) & 0xff, testNumber & 0xff);
    reg.write16(RegID_HL, testNumber);
    EXPECT_EQ(reg.read16(RegID_HL), testNumber);
    EXPECT_EQ(reg.read8(RegID_H), (testNumber >> 8) & 0xff);
    EXPECT_EQ(reg.read8(RegID_L), testNumber & 0xff);

    /* Register SP */
    testNumber = 0xbb2b;
    reg.write16(RegID_SP, testNumber);
    EXPECT_EQ(reg.read16(RegID_SP), testNumber);
    EXPECT_EQ(reg.getStackPointer(), testNumber);
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
