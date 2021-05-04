#include <gtest/gtest.h>
#include "polarGB/types.h"
#include "polarGB/register.h"


/**
 * Notes about the register class:
 * The register has multiple pairs: AF, BC, DE, HL
 * Special registers: SP, PC    (Always 16 bits)
 * Individual registers: A, F, B, C, D, E, H and L
 * The first letter of a register pair is the high byte while the last letter is the low byte.
 * For example storing the number 0x45bf in register BC will store B<-0x45 and C<-0xbf.
 */


TEST(RegisterTest, ReadWrite1Byte)
{
    Register reg;
    u8 testByte = 0x81;

    /* Register A */
    reg.write(RegID_A, testByte);
    EXPECT_EQ(reg.read(RegID_A), testByte);

    /* Register B */
    testByte = 0x15;
    reg.write(RegID_B, testByte);
    EXPECT_EQ(reg.read(RegID_B), testByte);

    /* Register C */
    testByte = 0xf4;
    reg.write(RegID_C, testByte);
    EXPECT_EQ(reg.read(RegID_C), testByte);

    /* Register D */
    testByte = 0x7f;
    reg.write(RegID_D, testByte);
    EXPECT_EQ(reg.read(RegID_D), testByte);

    /* Register E */
    testByte = 0xb6;
    reg.write(RegID_E, testByte);
    EXPECT_EQ(reg.read(RegID_E), testByte);

    /* Register H */
    testByte = 0xb8;
    reg.write(RegID_H, testByte);
    EXPECT_EQ(reg.read(RegID_H), testByte);

    /* Register L */
    testByte = 0xc8;
    reg.write(RegID_L, testByte);
    EXPECT_EQ(reg.read(RegID_L), testByte);
}


TEST(RegisterTest, ReadWrite2Bytes)
{
    Register reg;

    /* Register AF */
    u16 testNumber = 0x561c;
    u16 expectedOutput = testNumber & 0xfff0; /* bits 0-4 are always zero for the flag register. */
    reg.write(RegID_AF, testNumber);
    EXPECT_EQ(reg.read(RegID_AF), expectedOutput);
    EXPECT_EQ(reg.read(RegID_A), (expectedOutput >> 8) & 0xff);

    /* Register BC */
    testNumber = 0xc074;
    reg.write(RegID_BC, testNumber);
    EXPECT_EQ(reg.read(RegID_BC), testNumber);
    EXPECT_EQ(reg.read(RegID_B), (testNumber >> 8) & 0xff);
    EXPECT_EQ(reg.read(RegID_C), testNumber & 0xff);

    /* Register DE */
    testNumber = 0xb624;
    reg.write(RegID_DE, testNumber);
    EXPECT_EQ(reg.read(RegID_DE), testNumber);
    EXPECT_EQ(reg.read(RegID_D), (testNumber >> 8) & 0xff);
    EXPECT_EQ(reg.read(RegID_E), testNumber & 0xff);

    /* Register HL */
    testNumber = 0x46fc;
    reg.write(RegID_HL, testNumber);
    EXPECT_EQ(reg.read(RegID_HL), testNumber);
    EXPECT_EQ(reg.read(RegID_H), (testNumber >> 8) & 0xff);
    EXPECT_EQ(reg.read(RegID_L), testNumber & 0xff);

    /* Register SP */
    testNumber = 0xbb2b;
    reg.write(RegID_SP, testNumber);
    EXPECT_EQ(reg.read(RegID_SP), testNumber);
    EXPECT_EQ(reg.read(RegID_SP), testNumber);
}


TEST(RegisterTest, FlagsTrue)
{
    Register reg = Register();

    reg.setFlagZero(true);
    ASSERT_EQ(reg.getFlagZero(), true);
    ASSERT_EQ(reg.read(RegID_F) & 0x80, 0x80);

    reg.setFlagSub(true);
    ASSERT_EQ(reg.getFlagSub(), true);
    ASSERT_EQ(reg.read(RegID_F) & 0x40, 0x40);

    reg.setFlagHalfCarry(true);
    ASSERT_EQ(reg.getFlagHalfCarry(), true);
    ASSERT_EQ(reg.read(RegID_F) & 0x20, 0x20);

    reg.setFlagCarry(true);
    ASSERT_EQ(reg.getFlagCarry(), true);
    ASSERT_EQ(reg.read(RegID_F) & 0x10, 0x10);
}


TEST(RegisterTest, FlagsFalse)
{
    Register reg = Register();

    reg.setFlagZero(false);
    ASSERT_EQ(reg.getFlagZero(), false);
    ASSERT_EQ(reg.read(RegID_F) & 0x80, 0);

    reg.setFlagSub(false);
    ASSERT_EQ(reg.getFlagSub(), false);
    ASSERT_EQ(reg.read(RegID_F) & 0x40, 0);

    reg.setFlagHalfCarry(false);
    ASSERT_EQ(reg.getFlagHalfCarry(), false);
    ASSERT_EQ(reg.read(RegID_F) & 0x20, 0);

    reg.setFlagCarry(false);
    ASSERT_EQ(reg.getFlagCarry(), false);
    ASSERT_EQ(reg.read(RegID_F) & 0x10, 0);
}
