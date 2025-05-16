#pragma once

class Key {
public:
  int x;
  int y;
  Key(int x, int y) {
    this->x = x;
    this->y = y;
  }
};

enum ActionType {
  ACTION_NONE,
  ACTION_KEYCODE,
  ACTION_LAYER,
  ACTION_MACRO
};

struct Action {
  ActionType type;
  int value;

  constexpr Action()
    : type(ACTION_NONE), value(0) {}
  constexpr Action(ActionType type, int value)
    : type(type), value(value) {}
};

#define ____ Action()
#define KEY(k) Action(ACTION_KEYCODE, k)
#define LAY(l) Action(ACTION_LAYER, l)
#define MACRO(m) Action(ACTION_MACRO, m)