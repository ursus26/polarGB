#include <memory>
#include <gtest/gtest.h>
#include "polarGB/types.h"
#include "polarGB/interrupt_controller.h"
#include "polarGB/joypad.h"
#include "polarGB/timer.h"
#include "polarGB/mmu.h"



class MmuTest : public testing::Test
{
protected:
    void SetUp() override
    {
        ic = std::make_shared<InterruptController>();
        gc = std::make_shared<GraphicsController>(ic, true);
        joypad = std::make_shared<Joypad>(ic);
        timer = std::make_shared<Timer>(ic);
        mmu = std::make_shared<Mmu>(gc, ic, timer, joypad);
    }

    void TearDown() override
    {
        mmu->shutDown();
        gc->shutDown();
    }

    std::shared_ptr<GraphicsController> gc;
    std::shared_ptr<InterruptController> ic;
    std::shared_ptr<Joypad> joypad;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<Mmu> mmu;
};


/**************************************
 * GraphicsController RAM, 0x8000 - 0x9fff
 *************************************/
TEST_F(MmuTest, VRAMLowerBoundTest)
{
    u8 testValue = 0x55;
    u16 testAddr = VRAM_START_ADDR;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


TEST_F(MmuTest, VRAMGeneralCase)
{
    u8 testValue = 0xaa;
    u16 testAddr = VRAM_START_ADDR + 0x1000;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


TEST_F(MmuTest, VRAMUpperBoundTest)
{
    u8 testValue = 0xa5;
    u16 testAddr = VRAM_END_ADDR;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


/**************************************
 * Extended RAM, 0xa000 - 0xbfff
 *************************************/
TEST_F(MmuTest, ERAMLowerBoundTest)
{
    u8 testValue = 0x12;
    u16 testAddr = ERAM_START_ADDR;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


TEST_F(MmuTest, ERAMGeneralCase)
{
    u8 testValue = 0x23;
    u16 testAddr = ERAM_START_ADDR + 0x1200;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


TEST_F(MmuTest, ERAMUpperBoundTest)
{
    u8 testValue = 0x34;
    u16 testAddr = ERAM_END_ADDR;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


/**************************************
 * Working RAM, 0xc000 - 0xdfff
 *************************************/
TEST_F(MmuTest, WRAMLowerBoundTest)
{
    u8 testValue = 0x56;
    u16 testAddr = WRAM_START_ADDR;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


TEST_F(MmuTest, WRAMGeneralCase)
{
    u8 testValue = 0x67;
    u16 testAddr = WRAM_START_ADDR + 0x200;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


TEST_F(MmuTest, WRAMUpperBoundTest)
{
    u8 testValue = 0x78;
    u16 testAddr = WRAM_END_ADDR;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


/**************************************
 * OAM RAM, 0xfe00 - 0xfe9f
 *************************************/
TEST_F(MmuTest, OAMLowerBoundTest)
{
    u8 testValue = 0x03;
    u16 testAddr = OAM_START_ADDR;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


TEST_F(MmuTest, OAMGeneralCase)
{
    u8 testValue = 0x04;
    u16 testAddr = OAM_START_ADDR + 0x40;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


TEST_F(MmuTest, OAMUpperBoundTest)
{
    u8 testValue = 0x05;
    u16 testAddr = OAM_END_ADDR;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


/**************************************
 * Hardware registers, 0xff00 - 0xff7f
 *************************************/
TEST_F(MmuTest, HardwareRegistersLowerBoundTest)
{
    u8 testValue = 0xbb;
    u16 testAddr = HARDWARE_REGISTERS_START_ADDR;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


TEST_F(MmuTest, HardwareRegistersGeneralCase)
{
    u8 testValue = 0xcc;
    u16 testAddr = HARDWARE_REGISTERS_START_ADDR + 0x40;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


TEST_F(MmuTest, HardwareRegistersUpperBoundTest)
{
    u8 testValue = 0xdd;
    u16 testAddr = HARDWARE_REGISTERS_END_ADDR;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


/**************************************
 * HRAM, 0xff80 - 0xffff
 *************************************/
TEST_F(MmuTest, HRAMLowerBoundTest)
{
    u8 testValue = 0xb1;
    u16 testAddr = HRAM_START_ADDR;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


TEST_F(MmuTest, HRAMGeneralCase)
{
    u8 testValue = 0xc1;
    u16 testAddr = HRAM_START_ADDR + 0x40;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}


TEST_F(MmuTest, HRAMUpperBoundTest)
{
    u8 testValue = 0xd1;
    u16 testAddr = HRAM_END_ADDR;
    mmu->write(testAddr, testValue);
    ASSERT_EQ(mmu->read(testAddr), testValue);
}
