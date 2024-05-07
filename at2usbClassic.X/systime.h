#ifndef SYSTIME_H
#define SYSTIME_H

#include <stdint.h>

void SysTime_Init(void);
uint32_t SysTime_GetCurrentTimeMs(void);

#endif // SYSTIME_H