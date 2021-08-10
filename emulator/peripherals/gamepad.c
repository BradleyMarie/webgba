#include "emulator/peripherals/gamepad.h"

#include <stdlib.h>

struct _GamePad {
  GamePadToggleUpFunction toggle_up;
  GamePadToggleDownFunction toggle_down;
  GamePadToggleLeftFunction toggle_left;
  GamePadToggleRightFunction toggle_right;
  GamePadToggleAFunction toggle_a;
  GamePadToggleBFunction toggle_b;
  GamePadToggleLFunction toggle_l;
  GamePadToggleRFunction toggle_r;
  GamePadToggleStartFunction toggle_start;
  GamePadToggleSelectFunction toggle_select;
  GamePadContextFreeFunction free_context;
  void *context;
};

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
                         GamePadContextFreeFunction free_context) {
  GamePad *result = (GamePad *)malloc(sizeof(GamePad));
  if (result == NULL) {
    return NULL;
  }

  result->toggle_up = toggle_up;
  result->toggle_down = toggle_down;
  result->toggle_left = toggle_left;
  result->toggle_right = toggle_right;
  result->toggle_a = toggle_a;
  result->toggle_b = toggle_b;
  result->toggle_l = toggle_l;
  result->toggle_r = toggle_r;
  result->toggle_start = toggle_start;
  result->toggle_select = toggle_select;
  result->free_context = free_context;
  result->context = context;

  return result;
}

void GamePadToggleUp(GamePad *gamepad, bool pressed) {
  gamepad->toggle_up(gamepad->context, pressed);
}

void GamePadToggleDown(GamePad *gamepad, bool pressed) {
  gamepad->toggle_down(gamepad->context, pressed);
}

void GamePadToggleLeft(GamePad *gamepad, bool pressed) {
  gamepad->toggle_left(gamepad->context, pressed);
}

void GamePadToggleRight(GamePad *gamepad, bool pressed) {
  gamepad->toggle_right(gamepad->context, pressed);
}

void GamePadToggleA(GamePad *gamepad, bool pressed) {
  gamepad->toggle_a(gamepad->context, pressed);
}

void GamePadToggleB(GamePad *gamepad, bool pressed) {
  gamepad->toggle_b(gamepad->context, pressed);
}

void GamePadToggleL(GamePad *gamepad, bool pressed) {
  gamepad->toggle_l(gamepad->context, pressed);
}

void GamePadToggleR(GamePad *gamepad, bool pressed) {
  gamepad->toggle_r(gamepad->context, pressed);
}

void GamePadToggleStart(GamePad *gamepad, bool pressed) {
  gamepad->toggle_start(gamepad->context, pressed);
}

void GamePadToggleSelect(GamePad *gamepad, bool pressed) {
  gamepad->toggle_select(gamepad->context, pressed);
}

void GamePadFree(GamePad *gamepad) {
  if (gamepad->free_context) {
    gamepad->free_context(gamepad->context);
  }
  free(gamepad);
}