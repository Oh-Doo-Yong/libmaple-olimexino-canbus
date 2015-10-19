/*
 * olimexino-can.cpp
 *
 * Test Olimexino-stm32 CAN bus.
 *
 * This file is released into the public domain.
 */

#include <wirish/wirish.h>
#include "libraries/HardwareCAN/HardwareCAN.h"

using namespace std;

HardwareCAN canBus(CAN1_BASE);

void
setup()
{
    pinMode(BOARD_LED_PIN, OUTPUT);
    Serial1.begin(115200);
    SerialUSB.end();

    /* Olimexino layout - RX on PB8, TX on PB9 */
    canBus.map(CAN_GPIO_PB8_PB9);
    canBus.begin(CAN_SPEED_1000, CAN_MODE_NORMAL);
    canBus.filter(0, 0, 0);
    canBus.set_irq_mode();
}

void
PrintHex(uint8 b)
{
    static char digits[] = "0123456789ABCDEF";
    Serial1.print(digits[b >> 4]);
    Serial1.print(digits[b & 0xf]);
}

void
DumpMessage(CanMsg *msg)
{
    Serial1.print("> ");
    uint8 len = msg->DLC;
    for (uint8 i = 0; i < len; i++) {
        PrintHex(msg->Data[i]);
        Serial1.print(" ");
    }
    Serial1.print("\r\n");
    togglePin(BOARD_LED_PIN);
}

void
loop()
{
    /* Ensure you have enough CAN RX buffers for such code (CAN_RX_QUEUE_SIZE). */
    while(!canBus.available());
    CanMsg *r_msg = canBus.recv();
    if (r_msg) {
        DumpMessage(r_msg);
        canBus.free();
    }
}

// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated objects that need libmaple may fail.
__attribute__((constructor)) void
premain()
{
    init();
}

int
main(void)
{
    setup();

    while (1) {
        loop();
    }
    return 0;
}
