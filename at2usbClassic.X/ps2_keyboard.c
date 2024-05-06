#include "ps2_keyboard.h"
#include "systime.h"
#include "generic_queue.h"

#include "mcc_generated_files/mcc.h"


#define KBD_CLK_SetInterruptHandler     IOCAF4_SetInterruptHandler
#define KBD_DATA_SetInterruptHandler    IOCAF5_SetInterruptHandler

#define KEYBOARD_SCANCODE_BREAK     0xF0
#define KEYBOARD_SCANCODE_EXTEND    0xE0
#define KEYBOARD_RESET_TIMEOUT_MS   250

typedef enum KeyboardFsmState {
    KeyboardIdle,
    KeyboardStart,
    KeyboardData,
    KeyboardParity,
    KeyboardStop
} KeyboardFsmState;

static GenericQueue keyboardScanCodeQueue;
static volatile KeyboardFsmState keyboardState = KeyboardIdle;
static volatile uint8_t keyboardIndexData = 0;
static volatile uint8_t keyboardOnesCount = 0;
static volatile bool keyboardScanCodeValid = false;
static volatile uint8_t keyboardData = 0;
static PS2ScanCode keyboardScanCode = {};
static volatile bool keyboardIsExtend = false;
static volatile bool keyboardIsBreak = false;

static volatile uint8_t keyboardCommand = 0;
static volatile uint8_t keyboardCommandBits = 0;
static volatile uint8_t keyboardCommandCount = 0;
static volatile uint8_t keyboardCommandOnes = 0;

static volatile uint32_t prevTimeMs = 0;

static void PS2Keyboard_DataInterrupt(void);
static void PS2Keyboard_ClockInterrupt(void);

void PS2Keyboard_Init() {
    GenericQueue_Init(&keyboardScanCodeQueue, sizeof (PS2ScanCode));
    KBD_CLK_SetInterruptHandler(PS2Keyboard_ClockInterrupt);
    KBD_DATA_SetInterruptHandler(PS2Keyboard_DataInterrupt);
}

bool PS2Keyboard_GetScanCode(PS2ScanCode* scanCode) {
    bool ret = true;
    if (!GenericQueue_IsEmpty(&keyboardScanCodeQueue)) {
        GenericQueue_Dequeue(&keyboardScanCodeQueue, scanCode);
        ret = true;
    } else {
        ret = false;
    }
    return ret;
}

void PS2Keyboard_SendCommand(uint8_t cmd) {
    keyboardCommand = cmd;
    di();
    KBD_CLK_SetDigitalOutput();
    KBD_CLK_SetLow();
    __delay_us(100);
    KBD_CLK_SetDigitalInput();
    keyboardCommandCount = 0;
    keyboardCommandBits = 12;
    keyboardCommandOnes = 0;
    KBD_DATA_SetDigitalOutput();
    KBD_DATA_SetLow();
    ei();
}

uint8_t PS2Keyboard_GetLastCommand() {
    return keyboardCommand;
}

static void PS2Keyboard_DataInterrupt(void) {
    if (keyboardState == KeyboardIdle) {
        keyboardState = KeyboardStart;
        prevTimeMs = SysTime_GetCurrentTimeMs();
    }
}

static void PS2Keyboard_ClockInterrupt(void) {
    // check if we should send data to keyboard
    if (keyboardCommandBits > 0) {
        if(keyboardCommandCount > 0 && keyboardCommandCount < 9) {
            uint8_t bitPos = keyboardCommandCount - 1;
            bool bit = (keyboardCommand >> bitPos) & 1;
            if(bit) {
                KBD_DATA_SetHigh();
                keyboardCommandOnes += 1;
            } else {
                KBD_DATA_SetLow();
            }
        } else if(keyboardCommandCount == 9) {
            if(keyboardCommandOnes & 1) {
                KBD_DATA_SetLow();
            } else {
                KBD_DATA_SetHigh();
            }
        } else if(keyboardCommandCount == 10) {
            KBD_DATA_SetDigitalInput();
        }
        keyboardCommandCount += 1;
        keyboardCommandBits -= 1;
    } else {
        if (SysTime_GetCurrentTimeMs() - prevTimeMs >= KEYBOARD_RESET_TIMEOUT_MS) {
            keyboardState = KeyboardStart;
        }
        switch (keyboardState) {
            case KeyboardStart:
                keyboardIndexData = 0;
                keyboardOnesCount = 0;
                keyboardScanCode.value = 0;
                keyboardState = KeyboardData;
                break;
            case KeyboardData:
                keyboardData = KBD_DATA_GetValue();
                keyboardScanCode.value |= keyboardData << keyboardIndexData;
                keyboardOnesCount += keyboardData;
                keyboardIndexData += 1;
                if (keyboardIndexData == 8) {
                    keyboardState = KeyboardParity;
                } else {
                    keyboardState = KeyboardData;
                }
                break;
            case KeyboardParity:
                keyboardData = KBD_DATA_GetValue();
                keyboardOnesCount += keyboardData;
                keyboardScanCodeValid = keyboardOnesCount & 0x01;
                keyboardState = KeyboardStop;
                break;
            case KeyboardStop:
                if (keyboardScanCode.value == KEYBOARD_SCANCODE_EXTEND) {
                    keyboardIsExtend = true;
                } else if (keyboardScanCode.value == KEYBOARD_SCANCODE_BREAK) {
                    keyboardIsBreak = true;
                } else {
                    keyboardScanCode.isBreak = keyboardIsBreak;
                    keyboardScanCode.isExtend = keyboardIsExtend;
                    if (keyboardScanCodeValid) {
                        GenericQueue_Enqueue(&keyboardScanCodeQueue, &keyboardScanCode);
                    }
                    keyboardIsBreak = false;
                    keyboardIsExtend = false;
                }
                keyboardState = KeyboardIdle;
                break;
            default:
                keyboardState = KeyboardIdle;
                break;
        }
        prevTimeMs = SysTime_GetCurrentTimeMs();
    }
}