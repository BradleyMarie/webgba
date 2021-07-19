#ifndef _WEBGBA_EMULATOR_INTERRUPTS_INTERRUPT_LINE_
#define _WEBGBA_EMULATOR_INTERRUPTS_INTERRUPT_LINE_

#include <stdbool.h>

typedef bool (*InterruptLineIsTriggeredFunction)(const void *context);
typedef void (*InterruptLineContextFree)(void *context);

typedef struct _InterruptLine InterruptLine;
InterruptLine *InterruptLineAllocate(
    void *context, InterruptLineIsTriggeredFunction is_triggered,
    InterruptLineContextFree free_context);

bool InterruptLineIsTriggered(const InterruptLine *interrupt_line);

void InterruptLineFree(InterruptLine *interrupt_line);

#endif  // _WEBGBA_EMULATOR_INTERRUPTS_INTERRUPT_LINE_