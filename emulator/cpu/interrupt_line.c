#include "emulator/cpu/interrupt_line.h"

#include <stdlib.h>

struct _InterruptLine {
  InterruptLineSetLevelFunction set_level;
  InterruptLineContextFree free_context;
  void *context;
};

InterruptLine *InterruptLineAllocate(void *context,
                                     InterruptLineSetLevelFunction set_level,
                                     InterruptLineContextFree free_context) {
  InterruptLine *result = (InterruptLine *)malloc(sizeof(InterruptLine));
  if (result == NULL) {
    return NULL;
  }

  result->set_level = set_level;
  result->free_context = free_context;
  result->context = context;

  return result;
}

void InterruptLineSetLevel(InterruptLine *interrupt_line, bool raised) {
  return interrupt_line->set_level(interrupt_line->context, raised);
}

void InterruptLineFree(InterruptLine *interrupt_line) {
  if (interrupt_line->free_context) {
    interrupt_line->free_context(interrupt_line->context);
  }
  free(interrupt_line);
}