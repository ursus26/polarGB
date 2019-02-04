#include "interrupt_controller.h"


InterruptController::InterruptController()
{
    this->ime = false;
}


InterruptController::~InterruptController()
{

}


void InterruptController::disableInterrupts()
{
    this->ime = false;
}


void InterruptController::enableInterrupts()
{
    this->ime = true;
}
