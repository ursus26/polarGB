#include <gtest/gtest.h>
#include "polarGB/types.h"
#include "polarGB/mmu.h"


/**************************************
 * Video RAM, 0x8000 - 0x9fff
 *************************************/
TEST(MMUTests, VRAMLowerBoundTest)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0x55;
    u16 testAddr = VRAM_START_ADDR;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}


TEST(MMUTests, VRAMGeneralCase)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0xaa;
    u16 testAddr = VRAM_START_ADDR + 0x1000;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}


TEST(MMUTests, VRAMUpperBoundTest)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0xa5;
    u16 testAddr = VRAM_END_ADDR;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}

/**************************************
 * Extended RAM, 0xa000 - 0xbfff
 *************************************/
TEST(MMUTests, ERAMLowerBoundTest)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0x12;
    u16 testAddr = ERAM_START_ADDR;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}


TEST(MMUTests, ERAMGeneralCase)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0x23;
    u16 testAddr = ERAM_START_ADDR + 0x1200;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}


TEST(MMUTests, ERAMUpperBoundTest)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0x34;
    u16 testAddr = ERAM_END_ADDR;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}

/**************************************
 * Working RAM, 0xc000 - 0xdfff
 *************************************/
TEST(MMUTests, WRAMLowerBoundTest)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0x56;
    u16 testAddr = WRAM_START_ADDR;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}


TEST(MMUTests, WRAMGeneralCase)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0x67;
    u16 testAddr = WRAM_START_ADDR + 0x200;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}


TEST(MMUTests, WRAMUpperBoundTest)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0x78;
    u16 testAddr = WRAM_END_ADDR;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}


/**************************************
 * OAM RAM, 0xfe00 - 0xfe9f
 *************************************/
TEST(MMUTests, OAMLowerBoundTest)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0x03;
    u16 testAddr = OAM_START_ADDR;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}


TEST(MMUTests, OAMGeneralCase)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0x04;
    u16 testAddr = OAM_START_ADDR + 0x40;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}


TEST(MMUTests, OAMUpperBoundTest)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0x05;
    u16 testAddr = OAM_END_ADDR;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}


/**************************************
 * Hardware registers, 0xff00 - 0xff7f
 *************************************/
TEST(MMUTests, HardwareRegistersLowerBoundTest)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0xbb;
    u16 testAddr = HARDWARE_REGISTERS_START_ADDR;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}


TEST(MMUTests, HardwareRegistersGeneralCase)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0xcc;
    u16 testAddr = HARDWARE_REGISTERS_START_ADDR + 0x40;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}


TEST(MMUTests, HardwareRegistersUpperBoundTest)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0xdd;
    u16 testAddr = HARDWARE_REGISTERS_END_ADDR;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}


/**************************************
 * HRAM, 0xff80 - 0xffff
 *************************************/
TEST(MMUTests, HRAMLowerBoundTest)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0xb1;
    u16 testAddr = HRAM_START_ADDR;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}


TEST(MMUTests, HRAMGeneralCase)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0xc1;
    u16 testAddr = HRAM_START_ADDR + 0x40;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}


TEST(MMUTests, HRAMUpperBoundTest)
{
    Video v;
    v.startUp(true);
    Mmu mmu;
    mmu.startUp(&v);

    u8 testValue = 0xd1;
    u16 testAddr = HRAM_END_ADDR;
    mmu.write(testAddr, testValue);

    ASSERT_EQ(mmu.read(testAddr), testValue);

    mmu.shutDown();
    v.shutDown();
}
