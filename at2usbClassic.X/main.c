#include "mcc_generated_files/mcc.h"

#include "app_device_keyboard.h"
#include "systime.h"
#include "log.h"

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
    char scanCodeStr[6] = {' ', ' ', ' ', ' ', '\r', '\n'};
    
    while (1) {
        scanCodeValid = PS2Keyboard_GetScanCode(&scanCode);
        if(scanCodeValid) {
            hndlScanCode = &scanCode;
        } else {
            hndlScanCode = NULL;
        }
        if(hndlScanCode != NULL) {
            scanCodeStr[0] = hndlScanCode->isExtend ? 'e' : ' ';
            scanCodeStr[1] = hndlScanCode->isBreak ? 'b' : ' ';
            Log_UtilityConvertByteToHex(hndlScanCode->value, &scanCodeStr[2], 2);
            Log_Message((const uint8_t*)scanCodeStr, sizeof(scanCodeStr));
        }
        APP_KeyboardTasks(hndlScanCode);
    }
}