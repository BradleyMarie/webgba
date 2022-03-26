#include "emulator/platform/power.h"

#include <stdlib.h>

struct _Power {
  PowerSetFunction set;
  PowerContextFree free_context;
  void *context;
};

Power *PowerAllocate(void *context, PowerSetFunction set,
                     PowerContextFree free_context) {
  Power *result = (Power *)malloc(sizeof(Power));
  if (result == NULL) {
    return NULL;
  }

  result->set = set;
  result->free_context = free_context;
  result->context = context;

  return result;
}

void PowerSet(Power *power, PowerState state) {
  return power->set(power->context, state);
}

void PowerFree(Power *power) {
  if (power->free_context) {
    power->free_context(power->context);
  }
  free(power);
}