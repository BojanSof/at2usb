#ifndef LOG_H
#define	LOG_H

#include <stdint.h>

void Log_UtilityConvertByteToHex(uint8_t byte, char *buf, uint8_t bufSize);

void Log_Message(const uint8_t* buf, uint8_t bufSize);

#endif	/* LOG_H */