#include "types.h"
#include "USBHIDKeyboard.h"
// Note, must use version 1.12.3, later versions at least up to 1.12.5 are broken!!!!
#include <Adafruit_NeoPixel.h>


#define PIN 9
#define NUMPIXELS 21

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
const uint32_t magenta = pixels.Color(150, 0, 150);
const uint32_t red = pixels.Color(150, 0, 0);
const uint32_t white = pixels.Color(150, 150, 150);
const uint32_t teal = pixels.Color(0, 150, 150);
const uint32_t black = pixels.Color(0, 0, 0);

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

const uint32_t led_layer_map[5][4][6] = {
  {
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
  },
  {
    { black, black, black, black, black, black },
    { black, white, white, white, white, black },
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
  },
  {
    { red, red, red, red, red, red },
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
  },
  {
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
  },
  {
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
  }
};