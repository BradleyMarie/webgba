#ifndef _WEBGBA_EMULATOR_INTERRUPTS_INTERRUPT_LINE_
#define _WEBGBA_EMULATOR_INTERRUPTS_INTERRUPT_LINE_

#include <stdbool.h>

typedef bool (*InterruptLineIsRaisedFunction)(const void *context);
typedef void (*InterruptLineContextFree)(void *context);

typedef struct _InterruptLine InterruptLine;
InterruptLine *InterruptLineAllocate(void *context,
                                     InterruptLineIsRaisedFunction is_triggered,
                                     InterruptLineContextFree free_context);

bool InterruptLineIsRaised(const InterruptLine *interrupt_line);

void InterruptLineFree(InterruptLine *interrupt_line);

#endif  // _WEBGBA_EMULATOR_INTERRUPTS_INTERRUPT_LINE_