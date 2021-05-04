#include <gtest/gtest.h>
#include "polarGB/interrupt_controller.h"


TEST(InterruptTest, GenerateVBlankInterruptTest)
{
    InterruptController ic = InterruptController();
    ic.enableInterrupts(false);
    ic.setIF(int_vblank);

    /* Enable everything except vblank. */
    ic.setIE(~int_vblank);
    ASSERT_EQ(ic.checkInterrupts(false), 0);

    ic.setIE(int_vblank);
    ASSERT_EQ(ic.checkInterrupts(false), int_vblank);
}


TEST(InterruptTest, GenerateSTATInterruptTest)
{
    InterruptController ic = InterruptController();
    ic.enableInterrupts(false);
    ic.setIF(int_stat);

    ic.setIE(~int_stat);
    ASSERT_EQ(ic.checkInterrupts(false), 0);

    ic.setIE(int_stat);
    ASSERT_EQ(ic.checkInterrupts(false), int_stat);
}


TEST(InterruptTest, GenerateTimerInterruptTest)
{
    InterruptController ic = InterruptController();
    ic.enableInterrupts(false);
    ic.setIF(int_timer_overflow);

    ic.setIE(~int_timer_overflow);
    ASSERT_EQ(ic.checkInterrupts(false), 0);

    ic.setIE(int_timer_overflow);
    ASSERT_EQ(ic.checkInterrupts(false), int_timer_overflow);
}


TEST(InterruptTest, GenerateSerialInterruptTest)
{
    InterruptController ic = InterruptController();
    ic.enableInterrupts(false);
    ic.setIF(int_serial_transfer_completion);

    ic.setIE(~int_serial_transfer_completion);
    ASSERT_EQ(ic.checkInterrupts(false), 0);

    ic.setIE(int_serial_transfer_completion);
    ASSERT_EQ(ic.checkInterrupts(false), int_serial_transfer_completion);
}


TEST(InterruptTest, GenerateJoypadInterruptTest)
{
    InterruptController ic = InterruptController();
    ic.enableInterrupts(false);
    ic.setIF(int_joypad);

    ic.setIE(~int_joypad);
    ASSERT_EQ(ic.checkInterrupts(false), 0);

    ic.setIE(int_joypad);
    ASSERT_EQ(ic.checkInterrupts(false), int_joypad);
}


TEST(InterruptTest, GetInterruptAddress)
{
    InterruptController ic = InterruptController();
    ASSERT_EQ(ic.getInterruptVector(int_vblank), INTERRUPT_VERTICAL_BLANKING_ADDR);
    ASSERT_EQ(ic.getInterruptVector(int_stat), INTERRUPT_LCDC_ADDR);
    ASSERT_EQ(ic.getInterruptVector(int_timer_overflow), INTERRUPT_TIMER_OVERFLOW_ADDR);
    ASSERT_EQ(ic.getInterruptVector(int_serial_transfer_completion), INTERRUPT_SERIAL_TRANSFER_COMPLETION_ADDR);
    ASSERT_EQ(ic.getInterruptVector(int_joypad), INTERRUPT_JOYPAD_ADDR);
}
