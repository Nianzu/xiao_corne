#include "types.h"
#include "USBHIDKeyboard.h"

const Key key_map[5][5] = {
  { Key(5, 2), Key(5, 1), Key(4, 0), Key(4, 1), Key(5, 0) },
  { Key(2, 0), Key(1, 0), Key(0, 0), Key(3, 0), Key(3, 1) },
  { Key(3, 2), Key(2, 3), Key(2, 2), Key(1, 3), Key(4, 2) },
  { Key(2, 1), Key(0, 1), Key(0, 2), Key(1, 1), Key(1, 2) },
  { Key(0, 3), Key(-1, -1), Key(-1, -1), Key(-1, -1), Key(-1, -1) },
};

const Action action_map[5][4][6] = {
  {
    { KEY('y'), KEY('u'), KEY('i'), KEY('o'), KEY('p'), ____ },
    { KEY('h'), KEY('j'), KEY('k'), KEY('l'), KEY(';'), KEY(KEY_BACKSPACE) },
    { KEY('n'), KEY('m'), KEY(','), KEY('.'), ____, ____ },
    { LAY(2), KEY(KEY_KP_ENTER), KEY(KEY_LEFT_SHIFT), ____, ____, ____ },
  },
  {
    { ____, KEY(KEY_LEFT_ALT), KEY(KEY_TAB), ____, ____, ____ },
    { KEY(KEY_LEFT_GUI), KEY(KEY_LEFT_ARROW), KEY(KEY_DOWN_ARROW), KEY(KEY_UP_ARROW), KEY(KEY_RIGHT_ARROW), KEY(KEY_BACKSPACE) },
    { ____, KEY(KEY_HOME), ____, KEY(KEY_PRINT_SCREEN), KEY(KEY_END), ____ },
    { LAY(3), KEY(KEY_KP_ENTER), KEY(KEY_LEFT_SHIFT), ____, ____, ____ },
  },
  {
    { KEY('!'), KEY('<'), KEY('>'), KEY('='), KEY('&'), ____ },
    { KEY('?'), KEY('('), KEY(')'), KEY('\''), KEY(':'), KEY(KEY_BACKSPACE) },
    { KEY('+'), KEY('%'), KEY('\\'), KEY('@'), ____, ____ },
    { LAY(0), KEY(KEY_KP_ENTER), KEY(KEY_LEFT_SHIFT), ____, ____, ____ },
  },
  {
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
    { LAY(1), ____, ____, ____, ____, ____ },
  },
  {
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
  },
};
