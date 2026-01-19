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
  { Key(0, 0), Key(1, 0), Key(2, 0), Key(3, 0), Key(4, 0) },
  { Key(5, 0), Key(4, 1), Key(4, 2), Key(5, 2), Key(5, 1) },
  { Key(2, 2), Key(1, 1), Key(3, 2), Key(3, 1), Key(2, 1) },
  { Key(1, 2), Key(2, 3), Key(1, 3), Key(0, 1), Key(0, 2) },
  { Key(0, 3), Key(-1, -1), Key(-1, -1), Key(-1, -1), Key(-1, -1) },
};

const Action action_map[5][4][6] = {
  {
    { KEY('t'), KEY('r'), KEY('e'), KEY('w'), KEY('q'), KEY(KEY_ESC) },
    { KEY('g'), KEY('f'), KEY('d'), KEY('s'), KEY('a'), KEY(KEY_TAB) },
    { KEY('b'), KEY('v'), KEY('c'), KEY('x'), KEY('z'), ____ },
    { LAY(1), KEY(KEY_SPACE), KEY(KEY_LEFT_CTRL), ____, ____, ____ },
  },
  {
    { ____, KEY('9'), KEY('8'), KEY('7'), ____, KEY(KEY_ESC) },
    { KEY(KEY_KP_ENTER), KEY('6'), KEY('5'), KEY('4'), KEY('0'), KEY(KEY_TAB) },
    { KEY('.') , KEY('3'), KEY('2'), KEY('1'), ____, ____ },
    { LAY(0), KEY(KEY_SPACE), KEY(KEY_LEFT_CTRL), ____, ____, ____ },
  },
  {
    { KEY('^'), KEY(']'), KEY('['), KEY('_'), KEY('\"'), KEY(KEY_ESC) },
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

const uint32_t led_layer_map[5][4][6] = {
  {
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
  },
  {
    { black, white, white, white, black, black },
    { black, white, white, white, white, black },
    { black, white, white, white, black, black },
    { black, black, black, black, black, black },
  },
  {
    { red, red, red, red, red, red },
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
    { black, black, black, black, black, black },
  },
  {
    { black, black, red, black, black, black },
    { black, red, red, red, black, black },
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
