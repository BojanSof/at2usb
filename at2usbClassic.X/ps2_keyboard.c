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

static volatile uint32_t prevTimeMs = 0;

static void PS2KeyboardDataInterrupt(void);
static void PS2KeyboardClockInterrupt(void);

void PS2Keyboard_Init() {
    GenericQueue_Init(&keyboardScanCodeQueue, sizeof (PS2ScanCode));
    KBD_CLK_SetInterruptHandler(PS2KeyboardClockInterrupt);
    KBD_DATA_SetInterruptHandler(PS2KeyboardDataInterrupt);
}

bool PS2Keyboard_GetScanCode(PS2ScanCode* scanCode) {
    bool ret = true;
//    di();
    if (!GenericQueue_IsEmpty(&keyboardScanCodeQueue)) {
        GenericQueue_Dequeue(&keyboardScanCodeQueue, scanCode);
        ret = true;
    } else {
        ret = false;
    }
//    ei();
    return ret;
}

static void PS2KeyboardDataInterrupt(void) {
    if (keyboardState == KeyboardIdle) {
        keyboardState = KeyboardStart;
    }
}

static void PS2KeyboardClockInterrupt(void) {
    if (SysTime_GetCurrentTimeMs() - prevTimeMs >= KEYBOARD_RESET_TIMEOUT_MS) {
        keyboardState = KeyboardStart;
        prevTimeMs = SysTime_GetCurrentTimeMs();
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
}