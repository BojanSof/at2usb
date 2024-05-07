#include "mcc_generated_files/mcc.h"

#include "app_device_keyboard.h"
#include "systime.h"

#include <string.h>

/*
    Main application
 */

int main(void) {
    SYSTEM_Initialize();
    SysTime_Init();
    PS2Keyboard_Init();


    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts 
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts 
    // Use the following macros to: 

    // Enable the Global Interrupts 
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts 
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Peripheral Interrupts 
    //INTERRUPT_PeripheralInterruptDisable();
    
    PS2ScanCode scanCode = {0};
    bool scanCodeValid = false;
    PS2ScanCode *hndlScanCode = NULL;
    char scanCodeStr[10] = "";
    uint8_t strLen = 0;
    uint8_t i = 0;
    
    while (1) {
        scanCodeValid = PS2Keyboard_GetScanCode(&scanCode);
        if(scanCodeValid) {
            hndlScanCode = &scanCode;
        } else {
            hndlScanCode = NULL;
        }
        if(hndlScanCode != NULL) {
            strLen = (uint8_t)snprintf(scanCodeStr, sizeof(scanCodeStr), "%c%02X\r\n", hndlScanCode->isBreak ? 'b' : ' ', hndlScanCode->value);
            for(i = 0; i < strLen; i++) {
                while(!EUSART_is_tx_ready());
                EUSART_Write((uint8_t)scanCodeStr[i]);
            }
        }
        APP_KeyboardTasks(hndlScanCode);
    }
}