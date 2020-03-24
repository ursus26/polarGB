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


TEST(RegisterTest, WriteAndRead8bits)
{
    Register reg;
    u8 testByte = 0x81;

    /* Register A */
    reg.writeSingle(RegID_A, testByte);
    EXPECT_EQ(reg.readSingle(RegID_A), testByte);

    /* Register B */
    testByte = 0x15;
    reg.writeSingle(RegID_B, testByte);
    EXPECT_EQ(reg.readSingle(RegID_B), testByte);

    /* Register C */
    testByte = 0xf4;
    reg.writeSingle(RegID_C, testByte);
    EXPECT_EQ(reg.readSingle(RegID_C), testByte);

    /* Register D */
    testByte = 0x7f;
    reg.writeSingle(RegID_D, testByte);
    EXPECT_EQ(reg.readSingle(RegID_D), testByte);

    /* Register E */
    testByte = 0xb6;
    reg.writeSingle(RegID_E, testByte);
    EXPECT_EQ(reg.readSingle(RegID_E), testByte);

    /* Register H */
    testByte = 0xb8;
    reg.writeSingle(RegID_H, testByte);
    EXPECT_EQ(reg.readSingle(RegID_H), testByte);

    /* Register L */
    testByte = 0xc8;
    reg.writeSingle(RegID_L, testByte);
    EXPECT_EQ(reg.readSingle(RegID_L), testByte);
}


TEST(RegisterTest, WriteAndRead16bits)
{
    Register reg;

    /* Register AF */
    u16 testNumber = 0x561c;
    reg.writeDouble(RegID_AF, testNumber);
    EXPECT_EQ(reg.readDouble(RegID_AF), testNumber);
    EXPECT_EQ(reg.readSingle(RegID_A), (testNumber >> 8) & 0xff);

    /* Register BC */
    testNumber = 0xc074;
    reg.writeDouble(RegID_BC, testNumber);
    EXPECT_EQ(reg.readDouble(RegID_BC), testNumber);
    EXPECT_EQ(reg.readSingle(RegID_B), (testNumber >> 8) & 0xff);
    EXPECT_EQ(reg.readSingle(RegID_C), testNumber & 0xff);

    /* Register DE */
    testNumber = 0xb624;
    reg.writeDouble(RegID_DE, testNumber);
    EXPECT_EQ(reg.readDouble(RegID_DE), testNumber);
    EXPECT_EQ(reg.readSingle(RegID_D), (testNumber >> 8) & 0xff);
    EXPECT_EQ(reg.readSingle(RegID_E), testNumber & 0xff);

    /* Register HL */
    testNumber = 0x46fc;
    reg.writeDouble(RegID_HL, testNumber);
    EXPECT_EQ(reg.readDouble(RegID_HL), testNumber);
    EXPECT_EQ(reg.readSingle(RegID_H), (testNumber >> 8) & 0xff);
    EXPECT_EQ(reg.readSingle(RegID_L), testNumber & 0xff);

    /* Register SP */
    testNumber = 0xbb2b;
    reg.writeDouble(RegID_SP, testNumber);
    EXPECT_EQ(reg.readDouble(RegID_SP), testNumber);
    EXPECT_EQ(reg.getStackPointer(), testNumber);
}
