#ifndef _WEBGBA_EMULATOR_PERIPHERALS_GAMEPAD_
#define _WEBGBA_EMULATOR_PERIPHERALS_GAMEPAD_

#include <stdbool.h>

typedef void (*GamePadToggleUpFunction)(void *context, bool pressed);
typedef void (*GamePadToggleDownFunction)(void *context, bool pressed);
typedef void (*GamePadToggleLeftFunction)(void *context, bool pressed);
typedef void (*GamePadToggleRightFunction)(void *context, bool pressed);
typedef void (*GamePadToggleAFunction)(void *context, bool pressed);
typedef void (*GamePadToggleBFunction)(void *context, bool pressed);
typedef void (*GamePadToggleLFunction)(void *context, bool pressed);
typedef void (*GamePadToggleRFunction)(void *context, bool pressed);
typedef void (*GamePadToggleStartFunction)(void *context, bool pressed);
typedef void (*GamePadToggleSelectFunction)(void *context, bool pressed);
typedef void (*GamePadContextFreeFunction)(void *context);

typedef struct _GamePad GamePad;
GamePad *GamePadAllocate(void *context, GamePadToggleUpFunction toggle_up,
                         GamePadToggleDownFunction toggle_down,
                         GamePadToggleLeftFunction toggle_left,
                         GamePadToggleRightFunction toggle_right,
                         GamePadToggleAFunction toggle_a,
                         GamePadToggleBFunction toggle_b,
                         GamePadToggleLFunction toggle_l,
                         GamePadToggleRFunction toggle_r,
                         GamePadToggleStartFunction toggle_start,
                         GamePadToggleSelectFunction toggle_select,
                         GamePadContextFreeFunction free_context);

void GamePadToggleUp(GamePad *gamepad, bool pressed);
void GamePadToggleDown(GamePad *gamepad, bool pressed);
void GamePadToggleLeft(GamePad *gamepad, bool pressed);
void GamePadToggleRight(GamePad *gamepad, bool pressed);
void GamePadToggleA(GamePad *gamepad, bool pressed);
void GamePadToggleB(GamePad *gamepad, bool pressed);
void GamePadToggleL(GamePad *gamepad, bool pressed);
void GamePadToggleR(GamePad *gamepad, bool pressed);
void GamePadToggleStart(GamePad *gamepad, bool pressed);
void GamePadToggleSelect(GamePad *gamepad, bool pressed);

void GamePadFree(GamePad *gamepad);

#endif  // _WEBGBA_EMULATOR_PERIPHERALS_GAMEPAD_