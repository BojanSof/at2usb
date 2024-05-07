#include "log.h"
#include "mcc_generated_files/eusart.h"

static char hex2str(uint8_t val) {
    if (val < 10) {
        return val + '0';
    } else if (val < 16) {
        return val - 10 + 'A';
    } else {
        return 'X';
    }
}

void Log_UtilityConvertByteToHex(uint8_t byte, char *buf, uint8_t bufSize) {
    if (bufSize < 2) {
        return;
    }
    uint8_t d1 = byte / 16;
    uint8_t d0 = byte - d1 * 16;
    buf[0] = hex2str(d1);
    buf[1] = hex2str(d0);
}

void Log_Message(const uint8_t* buf, uint8_t bufSize) {
    for (uint8_t i = 0; i < bufSize; i++) {
        while (!EUSART_is_tx_ready());
        EUSART_Write(buf[i]);
    }
}