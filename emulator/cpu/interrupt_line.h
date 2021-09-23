#ifndef _WEBGBA_EMULATOR_CPU_INTERRUPT_LINE_
#define _WEBGBA_EMULATOR_CPU_INTERRUPT_LINE_

#include <stdbool.h>

typedef void (*InterruptLineSetLevelFunction)(void *context, bool raised);
typedef void (*InterruptLineContextFree)(void *context);

typedef struct _InterruptLine InterruptLine;
InterruptLine *InterruptLineAllocate(void *context,
                                     InterruptLineSetLevelFunction set_level,
                                     InterruptLineContextFree free_context);

void InterruptLineSetLevel(InterruptLine *interrupt_line, bool raised);

void InterruptLineFree(InterruptLine *interrupt_line);

#endif  // _WEBGBA_EMULATOR_CPU_INTERRUPT_LINE_