#ifndef PS2USB_H
#define	PS2USB_H

#include "ps2_keyboard.h"

#include <stdint.h>

uint8_t PS2USB_ScanCodeToUSBHID(const PS2ScanCode *scanCode);

#endif	/* PS2USB_H */

