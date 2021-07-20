#include "emulator/interrupts/interrupt_line.h"

#include <stdlib.h>

struct _InterruptLine {
  InterruptLineIsTriggeredFunction is_triggered;
  InterruptLineContextFree free_context;
  void *context;
};

InterruptLine *InterruptLineAllocate(
    void *context, InterruptLineIsTriggeredFunction is_triggered,
    InterruptLineContextFree free_context) {
  InterruptLine *result = (InterruptLine *)malloc(sizeof(InterruptLine));
  if (result == NULL) {
    return NULL;
  }

  result->is_triggered = is_triggered;
  result->free_context = free_context;
  result->context = context;

  return result;
}

bool InterruptLineIsTriggered(const InterruptLine *interrupt_line) {
  return interrupt_line->is_triggered(interrupt_line->context);
}

void InterruptLineFree(InterruptLine *interrupt_line) {
  if (interrupt_line->free_context) {
    interrupt_line->free_context(interrupt_line->context);
  }
  free(interrupt_line);
}