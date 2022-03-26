#ifndef _WEBGBA_EMULATOR_PLATFORM_POWER_
#define _WEBGBA_EMULATOR_PLATFORM_POWER_

typedef enum {
  POWER_STATE_RUN = 0,
  POWER_STATE_HALT = 1,
  POWER_STATE_STOP = 2
} PowerState;

typedef void (*PowerSetFunction)(void *context, PowerState state);
typedef void (*PowerContextFree)(void *context);

typedef struct _Power Power;
Power *PowerAllocate(void *context, PowerSetFunction set,
                     PowerContextFree free_context);

void PowerSet(Power *power, PowerState state);

void PowerFree(Power *power);

#endif  // _WEBGBA_EMULATOR_PLATFORM_POWER_