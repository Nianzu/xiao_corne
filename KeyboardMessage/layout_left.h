#include "types.h"
#include "USBHIDKeyboard.h"

const Key key_map[5][5] = {
  { Key(0, 0), Key(1, 0), Key(2, 0), Key(3, 0), Key(4, 0) },
  { Key(5, 0), Key(4, 1), Key(4, 2), Key(5, 2), Key(5, 1) },
  { Key(2, 2), Key(1, 1), Key(3, 2), Key(3, 1), Key(2, 1) },
  { Key(1, 2), Key(2, 3), Key(1, 3), Key(0, 1), Key(0, 2) },
  { Key(0, 3), Key(-1, -1), Key(-1, -1), Key(-1, -1), Key(-1, -1) },
};

const Action action_map[5][4][6] = {
  {
    { KEY('t'), KEY('r'), KEY('e'), KEY('w'), KEY('q'), ____ },
    { KEY('g'), KEY('f'), KEY('d'), KEY('s'), KEY('a'), KEY(KEY_TAB) },
    { KEY('b'), KEY('v'), KEY('c'), KEY('x'), KEY('z'), ____ },
    { LAY(1), KEY(KEY_SPACE), KEY(KEY_LEFT_CTRL), ____, ____, ____ },
  },
  {
    { ____, KEY('9'), KEY('8'), KEY('7'), ____, ____ },
    { ____, KEY('6'), KEY('5'), KEY('4'), KEY('0'), ____ },
    { ____, KEY('3'), KEY('2'), KEY('1'), ____, ____ },
    { LAY(0), KEY(KEY_SPACE), KEY(KEY_LEFT_CTRL), ____, ____, ____ },
  },
  {
    { KEY('^'), KEY(']'), KEY('['), KEY('_'), KEY('\"'), ____ },
    { KEY('*'), KEY('}'), KEY('{'), KEY('-'), KEY('/'), ____ },
    { KEY('`'), KEY('~'), KEY('|'), KEY('$'), KEY('#'), ____ },
    { LAY(3), KEY(KEY_SPACE), KEY(KEY_LEFT_CTRL), ____, ____, ____ },
  },
  {
    { KEY('r'), KEY('e'), KEY('w'), KEY('q'), ____, ____ },
    { KEY('f'), KEY('d'), KEY('s'), KEY('a'), KEY(KEY_LEFT_SHIFT), KEY(KEY_TAB) },
    { KEY('v'), KEY('c'), KEY('x'), KEY('z'), ____, ____ },
    { LAY(2), KEY(KEY_SPACE), KEY(KEY_LEFT_CTRL), ____, ____, ____ },
  },
  {
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
  }
};