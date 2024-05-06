#include "systime.h"

#include "mcc_generated_files/mcc.h"

#define SysTime_SetInterruptHandler TMR2_SetInterruptHandler
#define SysTime_Start TMR2_StartTimer

static uint32_t timeMs = 0;

static void SysTimeInterrupt(void);

void SysTime_Init(void)
{
    SysTime_SetInterruptHandler(SysTimeInterrupt);
    SysTime_Start();

}

uint32_t SysTime_GetCurrentTimeMs(void)
{
    return timeMs;
}

static void SysTimeInterrupt(void)
{
    ++timeMs;
}