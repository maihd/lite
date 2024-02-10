#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#endif

#include "lite_mouse.h"
#include "lite_memory.h"
#include "lite_string.h"
#include "lite_window.h"
#include "lite_keycode.h"
#include "lite_scancode.h"
#include "lite_renderer.h"

enum { LITE_EVENT_QUEUE_SIZE = 64 };

static HWND             s_window;

static HDC              s_hDC;
static HDC              s_hSurface;
static HBITMAP          s_hSurfaceBitmap;

static void*            s_surface_pixels;
static int32_t          s_surface_width;
static int32_t          s_surface_height;

static LiteEvent        s_events_queue[LITE_EVENT_QUEUE_SIZE];
static int32_t          s_events_queue_head = 0;
static int32_t          s_events_queue_tail = 0;

static LiteArena*       s_events_arena      = nullptr;
static LiteArenaTemp    s_events_arena_temp = { 0 };


static const LiteKeyCode lite_default_keymap[LITE_NUM_SCANCODES] = {
    /* 0 */ 0,
    /* 1 */ 0,
    /* 2 */ 0,
    /* 3 */ 0,
    /* 4 */ 'a',
    /* 5 */ 'b',
    /* 6 */ 'c',
    /* 7 */ 'd',
    /* 8 */ 'e',
    /* 9 */ 'f',
    /* 10 */ 'g',
    /* 11 */ 'h',
    /* 12 */ 'i',
    /* 13 */ 'j',
    /* 14 */ 'k',
    /* 15 */ 'l',
    /* 16 */ 'm',
    /* 17 */ 'n',
    /* 18 */ 'o',
    /* 19 */ 'p',
    /* 20 */ 'q',
    /* 21 */ 'r',
    /* 22 */ 's',
    /* 23 */ 't',
    /* 24 */ 'u',
    /* 25 */ 'v',
    /* 26 */ 'w',
    /* 27 */ 'x',
    /* 28 */ 'y',
    /* 29 */ 'z',
    /* 30 */ '1',
    /* 31 */ '2',
    /* 32 */ '3',
    /* 33 */ '4',
    /* 34 */ '5',
    /* 35 */ '6',
    /* 36 */ '7',
    /* 37 */ '8',
    /* 38 */ '9',
    /* 39 */ '0',
    /* 40 */ LiteKeyCode_RETURN,
    /* 41 */ LiteKeyCode_ESCAPE,
    /* 42 */ LiteKeyCode_BACKSPACE,
    /* 43 */ LiteKeyCode_TAB,
    /* 44 */ LiteKeyCode_SPACE,
    /* 45 */ '-',
    /* 46 */ '=',
    /* 47 */ '[',
    /* 48 */ ']',
    /* 49 */ '\\',
    /* 50 */ '#',
    /* 51 */ ';',
    /* 52 */ '\'',
    /* 53 */ '`',
    /* 54 */ ',',
    /* 55 */ '.',
    /* 56 */ '/',
    /* 57 */ LiteKeyCode_CAPSLOCK,
    /* 58 */ LiteKeyCode_F1,
    /* 59 */ LiteKeyCode_F2,
    /* 60 */ LiteKeyCode_F3,
    /* 61 */ LiteKeyCode_F4,
    /* 62 */ LiteKeyCode_F5,
    /* 63 */ LiteKeyCode_F6,
    /* 64 */ LiteKeyCode_F7,
    /* 65 */ LiteKeyCode_F8,
    /* 66 */ LiteKeyCode_F9,
    /* 67 */ LiteKeyCode_F10,
    /* 68 */ LiteKeyCode_F11,
    /* 69 */ LiteKeyCode_F12,
    /* 70 */ LiteKeyCode_PRINTSCREEN,
    /* 71 */ LiteKeyCode_SCROLLLOCK,
    /* 72 */ LiteKeyCode_PAUSE,
    /* 73 */ LiteKeyCode_INSERT,
    /* 74 */ LiteKeyCode_HOME,
    /* 75 */ LiteKeyCode_PAGEUP,
    /* 76 */ LiteKeyCode_DELETE,
    /* 77 */ LiteKeyCode_END,
    /* 78 */ LiteKeyCode_PAGEDOWN,
    /* 79 */ LiteKeyCode_RIGHT,
    /* 80 */ LiteKeyCode_LEFT,
    /* 81 */ LiteKeyCode_DOWN,
    /* 82 */ LiteKeyCode_UP,
    /* 83 */ LiteKeyCode_NUMLOCKCLEAR,
    /* 84 */ LiteKeyCode_KP_DIVIDE,
    /* 85 */ LiteKeyCode_KP_MULTIPLY,
    /* 86 */ LiteKeyCode_KP_MINUS,
    /* 87 */ LiteKeyCode_KP_PLUS,
    /* 88 */ LiteKeyCode_KP_ENTER,
    /* 89 */ LiteKeyCode_KP_1,
    /* 90 */ LiteKeyCode_KP_2,
    /* 91 */ LiteKeyCode_KP_3,
    /* 92 */ LiteKeyCode_KP_4,
    /* 93 */ LiteKeyCode_KP_5,
    /* 94 */ LiteKeyCode_KP_6,
    /* 95 */ LiteKeyCode_KP_7,
    /* 96 */ LiteKeyCode_KP_8,
    /* 97 */ LiteKeyCode_KP_9,
    /* 98 */ LiteKeyCode_KP_0,
    /* 99 */ LiteKeyCode_KP_PERIOD,
    /* 100 */ 0,
    /* 101 */ LiteKeyCode_APPLICATION,
    /* 102 */ LiteKeyCode_POWER,
    /* 103 */ LiteKeyCode_KP_EQUALS,
    /* 104 */ LiteKeyCode_F13,
    /* 105 */ LiteKeyCode_F14,
    /* 106 */ LiteKeyCode_F15,
    /* 107 */ LiteKeyCode_F16,
    /* 108 */ LiteKeyCode_F17,
    /* 109 */ LiteKeyCode_F18,
    /* 110 */ LiteKeyCode_F19,
    /* 111 */ LiteKeyCode_F20,
    /* 112 */ LiteKeyCode_F21,
    /* 113 */ LiteKeyCode_F22,
    /* 114 */ LiteKeyCode_F23,
    /* 115 */ LiteKeyCode_F24,
    /* 116 */ LiteKeyCode_EXECUTE,
    /* 117 */ LiteKeyCode_HELP,
    /* 118 */ LiteKeyCode_MENU,
    /* 119 */ LiteKeyCode_SELECT,
    /* 120 */ LiteKeyCode_STOP,
    /* 121 */ LiteKeyCode_AGAIN,
    /* 122 */ LiteKeyCode_UNDO,
    /* 123 */ LiteKeyCode_CUT,
    /* 124 */ LiteKeyCode_COPY,
    /* 125 */ LiteKeyCode_PASTE,
    /* 126 */ LiteKeyCode_FIND,
    /* 127 */ LiteKeyCode_MUTE,
    /* 128 */ LiteKeyCode_VOLUMEUP,
    /* 129 */ LiteKeyCode_VOLUMEDOWN,
    /* 130 */ 0,
    /* 131 */ 0,
    /* 132 */ 0,
    /* 133 */ LiteKeyCode_KP_COMMA,
    /* 134 */ LiteKeyCode_KP_EQUALSAS400,
    /* 135 */ 0,
    /* 136 */ 0,
    /* 137 */ 0,
    /* 138 */ 0,
    /* 139 */ 0,
    /* 140 */ 0,
    /* 141 */ 0,
    /* 142 */ 0,
    /* 143 */ 0,
    /* 144 */ 0,
    /* 145 */ 0,
    /* 146 */ 0,
    /* 147 */ 0,
    /* 148 */ 0,
    /* 149 */ 0,
    /* 150 */ 0,
    /* 151 */ 0,
    /* 152 */ 0,
    /* 153 */ LiteKeyCode_ALTERASE,
    /* 154 */ LiteKeyCode_SYSREQ,
    /* 155 */ LiteKeyCode_CANCEL,
    /* 156 */ LiteKeyCode_CLEAR,
    /* 157 */ LiteKeyCode_PRIOR,
    /* 158 */ LiteKeyCode_RETURN2,
    /* 159 */ LiteKeyCode_SEPARATOR,
    /* 160 */ LiteKeyCode_OUT,
    /* 161 */ LiteKeyCode_OPER,
    /* 162 */ LiteKeyCode_CLEARAGAIN,
    /* 163 */ LiteKeyCode_CRSEL,
    /* 164 */ LiteKeyCode_EXSEL,
    /* 165 */ 0,
    /* 166 */ 0,
    /* 167 */ 0,
    /* 168 */ 0,
    /* 169 */ 0,
    /* 170 */ 0,
    /* 171 */ 0,
    /* 172 */ 0,
    /* 173 */ 0,
    /* 174 */ 0,
    /* 175 */ 0,
    /* 176 */ LiteKeyCode_KP_00,
    /* 177 */ LiteKeyCode_KP_000,
    /* 178 */ LiteKeyCode_THOUSANDSSEPARATOR,
    /* 179 */ LiteKeyCode_DECIMALSEPARATOR,
    /* 180 */ LiteKeyCode_CURRENCYUNIT,
    /* 181 */ LiteKeyCode_CURRENCYSUBUNIT,
    /* 182 */ LiteKeyCode_KP_LEFTPAREN,
    /* 183 */ LiteKeyCode_KP_RIGHTPAREN,
    /* 184 */ LiteKeyCode_KP_LEFTBRACE,
    /* 185 */ LiteKeyCode_KP_RIGHTBRACE,
    /* 186 */ LiteKeyCode_KP_TAB,
    /* 187 */ LiteKeyCode_KP_BACKSPACE,
    /* 188 */ LiteKeyCode_KP_A,
    /* 189 */ LiteKeyCode_KP_B,
    /* 190 */ LiteKeyCode_KP_C,
    /* 191 */ LiteKeyCode_KP_D,
    /* 192 */ LiteKeyCode_KP_E,
    /* 193 */ LiteKeyCode_KP_F,
    /* 194 */ LiteKeyCode_KP_XOR,
    /* 195 */ LiteKeyCode_KP_POWER,
    /* 196 */ LiteKeyCode_KP_PERCENT,
    /* 197 */ LiteKeyCode_KP_LESS,
    /* 198 */ LiteKeyCode_KP_GREATER,
    /* 199 */ LiteKeyCode_KP_AMPERSAND,
    /* 200 */ LiteKeyCode_KP_DBLAMPERSAND,
    /* 201 */ LiteKeyCode_KP_VERTICALBAR,
    /* 202 */ LiteKeyCode_KP_DBLVERTICALBAR,
    /* 203 */ LiteKeyCode_KP_COLON,
    /* 204 */ LiteKeyCode_KP_HASH,
    /* 205 */ LiteKeyCode_KP_SPACE,
    /* 206 */ LiteKeyCode_KP_AT,
    /* 207 */ LiteKeyCode_KP_EXCLAM,
    /* 208 */ LiteKeyCode_KP_MEMSTORE,
    /* 209 */ LiteKeyCode_KP_MEMRECALL,
    /* 210 */ LiteKeyCode_KP_MEMCLEAR,
    /* 211 */ LiteKeyCode_KP_MEMADD,
    /* 212 */ LiteKeyCode_KP_MEMSUBTRACT,
    /* 213 */ LiteKeyCode_KP_MEMMULTIPLY,
    /* 214 */ LiteKeyCode_KP_MEMDIVIDE,
    /* 215 */ LiteKeyCode_KP_PLUSMINUS,
    /* 216 */ LiteKeyCode_KP_CLEAR,
    /* 217 */ LiteKeyCode_KP_CLEARENTRY,
    /* 218 */ LiteKeyCode_KP_BINARY,
    /* 219 */ LiteKeyCode_KP_OCTAL,
    /* 220 */ LiteKeyCode_KP_DECIMAL,
    /* 221 */ LiteKeyCode_KP_HEXADECIMAL,
    /* 222 */ 0,
    /* 223 */ 0,
    /* 224 */ LiteKeyCode_LCTRL,
    /* 225 */ LiteKeyCode_LSHIFT,
    /* 226 */ LiteKeyCode_LALT,
    /* 227 */ LiteKeyCode_LGUI,
    /* 228 */ LiteKeyCode_RCTRL,
    /* 229 */ LiteKeyCode_RSHIFT,
    /* 230 */ LiteKeyCode_RALT,
    /* 231 */ LiteKeyCode_RGUI,
    /* 232 */ 0,
    /* 233 */ 0,
    /* 234 */ 0,
    /* 235 */ 0,
    /* 236 */ 0,
    /* 237 */ 0,
    /* 238 */ 0,
    /* 239 */ 0,
    /* 240 */ 0,
    /* 241 */ 0,
    /* 242 */ 0,
    /* 243 */ 0,
    /* 244 */ 0,
    /* 245 */ 0,
    /* 246 */ 0,
    /* 247 */ 0,
    /* 248 */ 0,
    /* 249 */ 0,
    /* 250 */ 0,
    /* 251 */ 0,
    /* 252 */ 0,
    /* 253 */ 0,
    /* 254 */ 0,
    /* 255 */ 0,
    /* 256 */ 0,
    /* 257 */ LiteKeyCode_MODE,
    /* 258 */ LiteKeyCode_AUDIONEXT,
    /* 259 */ LiteKeyCode_AUDIOPREV,
    /* 260 */ LiteKeyCode_AUDIOSTOP,
    /* 261 */ LiteKeyCode_AUDIOPLAY,
    /* 262 */ LiteKeyCode_AUDIOMUTE,
    /* 263 */ LiteKeyCode_MEDIASELECT,
    /* 264 */ LiteKeyCode_WWW,
    /* 265 */ LiteKeyCode_MAIL,
    /* 266 */ LiteKeyCode_CALCULATOR,
    /* 267 */ LiteKeyCode_COMPUTER,
    /* 268 */ LiteKeyCode_AC_SEARCH,
    /* 269 */ LiteKeyCode_AC_HOME,
    /* 270 */ LiteKeyCode_AC_BACK,
    /* 271 */ LiteKeyCode_AC_FORWARD,
    /* 272 */ LiteKeyCode_AC_STOP,
    /* 273 */ LiteKeyCode_AC_REFRESH,
    /* 274 */ LiteKeyCode_AC_BOOKMARKS,
    /* 275 */ LiteKeyCode_BRIGHTNESSDOWN,
    /* 276 */ LiteKeyCode_BRIGHTNESSUP,
    /* 277 */ LiteKeyCode_DISPLAYSWITCH,
    /* 278 */ LiteKeyCode_KBDILLUMTOGGLE,
    /* 279 */ LiteKeyCode_KBDILLUMDOWN,
    /* 280 */ LiteKeyCode_KBDILLUMUP,
    /* 281 */ LiteKeyCode_EJECT,
    /* 282 */ LiteKeyCode_SLEEP,
    /* 283 */ LiteKeyCode_APP1,
    /* 284 */ LiteKeyCode_APP2,
    /* 285 */ LiteKeyCode_AUDIOREWIND,
    /* 286 */ LiteKeyCode_AUDIOFASTFORWARD,
    /* 287 */ LiteKeyCode_SOFTLEFT,
    /* 288 */ LiteKeyCode_SOFTRIGHT,
    /* 289 */ LiteKeyCode_CALL,
    /* 290 */ LiteKeyCode_ENDCALL,
};


static const char* lite_scancode_names[LITE_NUM_SCANCODES] = {
    /* 0 */ nullptr,
    /* 1 */ nullptr,
    /* 2 */ nullptr,
    /* 3 */ nullptr,
    /* 4 */ "A",
    /* 5 */ "B",
    /* 6 */ "C",
    /* 7 */ "D",
    /* 8 */ "E",
    /* 9 */ "F",
    /* 10 */ "G",
    /* 11 */ "H",
    /* 12 */ "I",
    /* 13 */ "J",
    /* 14 */ "K",
    /* 15 */ "L",
    /* 16 */ "M",
    /* 17 */ "N",
    /* 18 */ "O",
    /* 19 */ "P",
    /* 20 */ "Q",
    /* 21 */ "R",
    /* 22 */ "S",
    /* 23 */ "T",
    /* 24 */ "U",
    /* 25 */ "V",
    /* 26 */ "W",
    /* 27 */ "X",
    /* 28 */ "Y",
    /* 29 */ "Z",
    /* 30 */ "1",
    /* 31 */ "2",
    /* 32 */ "3",
    /* 33 */ "4",
    /* 34 */ "5",
    /* 35 */ "6",
    /* 36 */ "7",
    /* 37 */ "8",
    /* 38 */ "9",
    /* 39 */ "0",
    /* 40 */ "Return",
    /* 41 */ "Escape",
    /* 42 */ "Backspace",
    /* 43 */ "Tab",
    /* 44 */ "Space",
    /* 45 */ "-",
    /* 46 */ "=",
    /* 47 */ "[",
    /* 48 */ "]",
    /* 49 */ "\\",
    /* 50 */ "#",
    /* 51 */ ";",
    /* 52 */ "'",
    /* 53 */ "`",
    /* 54 */ ",",
    /* 55 */ ".",
    /* 56 */ "/",
    /* 57 */ "CapsLock",
    /* 58 */ "F1",
    /* 59 */ "F2",
    /* 60 */ "F3",
    /* 61 */ "F4",
    /* 62 */ "F5",
    /* 63 */ "F6",
    /* 64 */ "F7",
    /* 65 */ "F8",
    /* 66 */ "F9",
    /* 67 */ "F10",
    /* 68 */ "F11",
    /* 69 */ "F12",
    /* 70 */ "PrintScreen",
    /* 71 */ "ScrollLock",
    /* 72 */ "Pause",
    /* 73 */ "Insert",
    /* 74 */ "Home",
    /* 75 */ "PageUp",
    /* 76 */ "Delete",
    /* 77 */ "End",
    /* 78 */ "PageDown",
    /* 79 */ "Right",
    /* 80 */ "Left",
    /* 81 */ "Down",
    /* 82 */ "Up",
    /* 83 */ "Numlock",
    /* 84 */ "Keypad /",
    /* 85 */ "Keypad *",
    /* 86 */ "Keypad -",
    /* 87 */ "Keypad +",
    /* 88 */ "Keypad Enter",
    /* 89 */ "Keypad 1",
    /* 90 */ "Keypad 2",
    /* 91 */ "Keypad 3",
    /* 92 */ "Keypad 4",
    /* 93 */ "Keypad 5",
    /* 94 */ "Keypad 6",
    /* 95 */ "Keypad 7",
    /* 96 */ "Keypad 8",
    /* 97 */ "Keypad 9",
    /* 98 */ "Keypad 0",
    /* 99 */ "Keypad .",
    /* 100 */ nullptr,
    /* 101 */ "Application",
    /* 102 */ "Power",
    /* 103 */ "Keypad =",
    /* 104 */ "F13",
    /* 105 */ "F14",
    /* 106 */ "F15",
    /* 107 */ "F16",
    /* 108 */ "F17",
    /* 109 */ "F18",
    /* 110 */ "F19",
    /* 111 */ "F20",
    /* 112 */ "F21",
    /* 113 */ "F22",
    /* 114 */ "F23",
    /* 115 */ "F24",
    /* 116 */ "Execute",
    /* 117 */ "Help",
    /* 118 */ "Menu",
    /* 119 */ "Select",
    /* 120 */ "Stop",
    /* 121 */ "Again",
    /* 122 */ "Undo",
    /* 123 */ "Cut",
    /* 124 */ "Copy",
    /* 125 */ "Paste",
    /* 126 */ "Find",
    /* 127 */ "Mute",
    /* 128 */ "VolumeUp",
    /* 129 */ "VolumeDown",
    /* 130 */ nullptr,
    /* 131 */ nullptr,
    /* 132 */ nullptr,
    /* 133 */ "Keypad ,",
    /* 134 */ "Keypad = (AS400)",
    /* 135 */ nullptr,
    /* 136 */ nullptr,
    /* 137 */ nullptr,
    /* 138 */ nullptr,
    /* 139 */ nullptr,
    /* 140 */ nullptr,
    /* 141 */ nullptr,
    /* 142 */ nullptr,
    /* 143 */ nullptr,
    /* 144 */ nullptr,
    /* 145 */ nullptr,
    /* 146 */ nullptr,
    /* 147 */ nullptr,
    /* 148 */ nullptr,
    /* 149 */ nullptr,
    /* 150 */ nullptr,
    /* 151 */ nullptr,
    /* 152 */ nullptr,
    /* 153 */ "AltErase",
    /* 154 */ "SysReq",
    /* 155 */ "Cancel",
    /* 156 */ "Clear",
    /* 157 */ "Prior",
    /* 158 */ "Return",
    /* 159 */ "Separator",
    /* 160 */ "Out",
    /* 161 */ "Oper",
    /* 162 */ "Clear / Again",
    /* 163 */ "CrSel",
    /* 164 */ "ExSel",
    /* 165 */ nullptr,
    /* 166 */ nullptr,
    /* 167 */ nullptr,
    /* 168 */ nullptr,
    /* 169 */ nullptr,
    /* 170 */ nullptr,
    /* 171 */ nullptr,
    /* 172 */ nullptr,
    /* 173 */ nullptr,
    /* 174 */ nullptr,
    /* 175 */ nullptr,
    /* 176 */ "Keypad 00",
    /* 177 */ "Keypad 000",
    /* 178 */ "ThousandsSeparator",
    /* 179 */ "DecimalSeparator",
    /* 180 */ "CurrencyUnit",
    /* 181 */ "CurrencySubUnit",
    /* 182 */ "Keypad (",
    /* 183 */ "Keypad )",
    /* 184 */ "Keypad {",
    /* 185 */ "Keypad }",
    /* 186 */ "Keypad Tab",
    /* 187 */ "Keypad Backspace",
    /* 188 */ "Keypad A",
    /* 189 */ "Keypad B",
    /* 190 */ "Keypad C",
    /* 191 */ "Keypad D",
    /* 192 */ "Keypad E",
    /* 193 */ "Keypad F",
    /* 194 */ "Keypad XOR",
    /* 195 */ "Keypad ^",
    /* 196 */ "Keypad %",
    /* 197 */ "Keypad <",
    /* 198 */ "Keypad >",
    /* 199 */ "Keypad &",
    /* 200 */ "Keypad &&",
    /* 201 */ "Keypad |",
    /* 202 */ "Keypad ||",
    /* 203 */ "Keypad :",
    /* 204 */ "Keypad #",
    /* 205 */ "Keypad Space",
    /* 206 */ "Keypad @",
    /* 207 */ "Keypad !",
    /* 208 */ "Keypad MemStore",
    /* 209 */ "Keypad MemRecall",
    /* 210 */ "Keypad MemClear",
    /* 211 */ "Keypad MemAdd",
    /* 212 */ "Keypad MemSubtract",
    /* 213 */ "Keypad MemMultiply",
    /* 214 */ "Keypad MemDivide",
    /* 215 */ "Keypad +/-",
    /* 216 */ "Keypad Clear",
    /* 217 */ "Keypad ClearEntry",
    /* 218 */ "Keypad Binary",
    /* 219 */ "Keypad Octal",
    /* 220 */ "Keypad Decimal",
    /* 221 */ "Keypad Hexadecimal",
    /* 222 */ nullptr,
    /* 223 */ nullptr,
    /* 224 */ "Left Ctrl",
    /* 225 */ "Left Shift",
    /* 226 */ "Left Alt",
    /* 227 */ "Left GUI",
    /* 228 */ "Right Ctrl",
    /* 229 */ "Right Shift",
    /* 230 */ "Right Alt",
    /* 231 */ "Right GUI",
    /* 232 */ nullptr,
    /* 233 */ nullptr,
    /* 234 */ nullptr,
    /* 235 */ nullptr,
    /* 236 */ nullptr,
    /* 237 */ nullptr,
    /* 238 */ nullptr,
    /* 239 */ nullptr,
    /* 240 */ nullptr,
    /* 241 */ nullptr,
    /* 242 */ nullptr,
    /* 243 */ nullptr,
    /* 244 */ nullptr,
    /* 245 */ nullptr,
    /* 246 */ nullptr,
    /* 247 */ nullptr,
    /* 248 */ nullptr,
    /* 249 */ nullptr,
    /* 250 */ nullptr,
    /* 251 */ nullptr,
    /* 252 */ nullptr,
    /* 253 */ nullptr,
    /* 254 */ nullptr,
    /* 255 */ nullptr,
    /* 256 */ nullptr,
    /* 257 */ "ModeSwitch",
    /* 258 */ "AudioNext",
    /* 259 */ "AudioPrev",
    /* 260 */ "AudioStop",
    /* 261 */ "AudioPlay",
    /* 262 */ "AudioMute",
    /* 263 */ "MediaSelect",
    /* 264 */ "WWW",
    /* 265 */ "Mail",
    /* 266 */ "Calculator",
    /* 267 */ "Computer",
    /* 268 */ "AC Search",
    /* 269 */ "AC Home",
    /* 270 */ "AC Back",
    /* 271 */ "AC Forward",
    /* 272 */ "AC Stop",
    /* 273 */ "AC Refresh",
    /* 274 */ "AC Bookmarks",
    /* 275 */ "BrightnessDown",
    /* 276 */ "BrightnessUp",
    /* 277 */ "DisplaySwitch",
    /* 278 */ "KBDIllumToggle",
    /* 279 */ "KBDIllumDown",
    /* 280 */ "KBDIllumUp",
    /* 281 */ "Eject",
    /* 282 */ "Sleep",
    /* 283 */ "App1",
    /* 284 */ "App2",
    /* 285 */ "AudioRewind",
    /* 286 */ "AudioFastForward",
    /* 287 */ "SoftLeft",
    /* 288 */ "SoftRight",
    /* 289 */ "Call",
    /* 290 */ "EndCall",
};


static const LiteScanCode lite_win32_scancode_table[] = {
    /*0x00*/ LiteScanCode_UNKNOWN,
    /*0x01*/ LiteScanCode_ESCAPE,
    /*0x02*/ LiteScanCode_1,
    /*0x03*/ LiteScanCode_2,
    /*0x04*/ LiteScanCode_3,
    /*0x05*/ LiteScanCode_4,
    /*0x06*/ LiteScanCode_5,
    /*0x07*/ LiteScanCode_6,
    /*0x08*/ LiteScanCode_7,
    /*0x09*/ LiteScanCode_8,
    /*0x0a*/ LiteScanCode_9,
    /*0x0b*/ LiteScanCode_0,
    /*0x0c*/ LiteScanCode_MINUS,
    /*0x0d*/ LiteScanCode_EQUALS,
    /*0x0e*/ LiteScanCode_BACKSPACE,
    /*0x0f*/ LiteScanCode_TAB,
    /*0x10*/ LiteScanCode_Q,
    /*0x11*/ LiteScanCode_W,
    /*0x12*/ LiteScanCode_E,
    /*0x13*/ LiteScanCode_R,
    /*0x14*/ LiteScanCode_T,
    /*0x15*/ LiteScanCode_Y,
    /*0x16*/ LiteScanCode_U,
    /*0x17*/ LiteScanCode_I,
    /*0x18*/ LiteScanCode_O,
    /*0x19*/ LiteScanCode_P,
    /*0x1a*/ LiteScanCode_LEFTBRACKET,
    /*0x1b*/ LiteScanCode_RIGHTBRACKET,
    /*0x1c*/ LiteScanCode_RETURN,
    /*0x1d*/ LiteScanCode_LCTRL,
    /*0x1e*/ LiteScanCode_A,
    /*0x1f*/ LiteScanCode_S,
    /*0x20*/ LiteScanCode_D,
    /*0x21*/ LiteScanCode_F,
    /*0x22*/ LiteScanCode_G,
    /*0x23*/ LiteScanCode_H,
    /*0x24*/ LiteScanCode_J,
    /*0x25*/ LiteScanCode_K,
    /*0x26*/ LiteScanCode_L,
    /*0x27*/ LiteScanCode_SEMICOLON,
    /*0x28*/ LiteScanCode_APOSTROPHE,
    /*0x29*/ LiteScanCode_GRAVE,
    /*0x2a*/ LiteScanCode_LSHIFT,
    /*0x2b*/ LiteScanCode_BACKSLASH,
    /*0x2c*/ LiteScanCode_Z,
    /*0x2d*/ LiteScanCode_X,
    /*0x2e*/ LiteScanCode_C,
    /*0x2f*/ LiteScanCode_V,
    /*0x30*/ LiteScanCode_B,
    /*0x31*/ LiteScanCode_N,
    /*0x32*/ LiteScanCode_M,
    /*0x33*/ LiteScanCode_COMMA,
    /*0x34*/ LiteScanCode_PERIOD,
    /*0x35*/ LiteScanCode_SLASH,
    /*0x36*/ LiteScanCode_RSHIFT,
    /*0x37*/ LiteScanCode_KP_MULTIPLY,
    /*0x38*/ LiteScanCode_LALT,
    /*0x39*/ LiteScanCode_SPACE,
    /*0x3a*/ LiteScanCode_CAPSLOCK,
    /*0x3b*/ LiteScanCode_F1,
    /*0x3c*/ LiteScanCode_F2,
    /*0x3d*/ LiteScanCode_F3,
    /*0x3e*/ LiteScanCode_F4,
    /*0x3f*/ LiteScanCode_F5,
    /*0x40*/ LiteScanCode_F6,
    /*0x41*/ LiteScanCode_F7,
    /*0x42*/ LiteScanCode_F8,
    /*0x43*/ LiteScanCode_F9,
    /*0x44*/ LiteScanCode_F10,
    /*0x45*/ LiteScanCode_NUMLOCKCLEAR,
    /*0x46*/ LiteScanCode_SCROLLLOCK,
    /*0x47*/ LiteScanCode_KP_7,
    /*0x48*/ LiteScanCode_KP_8,
    /*0x49*/ LiteScanCode_KP_9,
    /*0x4a*/ LiteScanCode_KP_MINUS,
    /*0x4b*/ LiteScanCode_KP_4,
    /*0x4c*/ LiteScanCode_KP_5,
    /*0x4d*/ LiteScanCode_KP_6,
    /*0x4e*/ LiteScanCode_KP_PLUS,
    /*0x4f*/ LiteScanCode_KP_1,
    /*0x50*/ LiteScanCode_KP_2,
    /*0x51*/ LiteScanCode_KP_3,
    /*0x52*/ LiteScanCode_KP_0,
    /*0x53*/ LiteScanCode_KP_PERIOD,
    /*0x54*/ LiteScanCode_UNKNOWN,
    /*0x55*/ LiteScanCode_UNKNOWN,
    /*0x56*/ LiteScanCode_NONUSBACKSLASH,
    /*0x57*/ LiteScanCode_F11,
    /*0x58*/ LiteScanCode_F12,
    /*0x59*/ LiteScanCode_KP_EQUALS,
    /*0x5a*/ LiteScanCode_UNKNOWN,
    /*0x5b*/ LiteScanCode_UNKNOWN,
    /*0x5c*/ LiteScanCode_INTERNATIONAL6,
    /*0x5d*/ LiteScanCode_UNKNOWN,
    /*0x5e*/ LiteScanCode_UNKNOWN,
    /*0x5f*/ LiteScanCode_UNKNOWN,
    /*0x60*/ LiteScanCode_UNKNOWN,
    /*0x61*/ LiteScanCode_UNKNOWN,
    /*0x62*/ LiteScanCode_UNKNOWN,
    /*0x63*/ LiteScanCode_UNKNOWN,
    /*0x64*/ LiteScanCode_F13,
    /*0x65*/ LiteScanCode_F14,
    /*0x66*/ LiteScanCode_F15,
    /*0x67*/ LiteScanCode_F16,
    /*0x68*/ LiteScanCode_F17,
    /*0x69*/ LiteScanCode_F18,
    /*0x6a*/ LiteScanCode_F19,
    /*0x6b*/ LiteScanCode_F20,
    /*0x6c*/ LiteScanCode_F21,
    /*0x6d*/ LiteScanCode_F22,
    /*0x6e*/ LiteScanCode_F23,
    /*0x6f*/ LiteScanCode_UNKNOWN,
    /*0x70*/ LiteScanCode_INTERNATIONAL2,
    /*0x71*/ LiteScanCode_LANG2,
    /*0x72*/ LiteScanCode_LANG1,
    /*0x73*/ LiteScanCode_INTERNATIONAL1,
    /*0x74*/ LiteScanCode_UNKNOWN,
    /*0x75*/ LiteScanCode_UNKNOWN,
    /*0x76*/ LiteScanCode_F24,
    /*0x77*/ LiteScanCode_LANG4,
    /*0x78*/ LiteScanCode_LANG3,
    /*0x79*/ LiteScanCode_INTERNATIONAL4,
    /*0x7a*/ LiteScanCode_UNKNOWN,
    /*0x7b*/ LiteScanCode_INTERNATIONAL5,
    /*0x7c*/ LiteScanCode_UNKNOWN,
    /*0x7d*/ LiteScanCode_INTERNATIONAL3,
    /*0x7e*/ LiteScanCode_KP_COMMA,
    /*0x7f*/ LiteScanCode_UNKNOWN,
    /*0xe000*/ LiteScanCode_UNKNOWN,
    /*0xe001*/ LiteScanCode_UNKNOWN,
    /*0xe002*/ LiteScanCode_UNKNOWN,
    /*0xe003*/ LiteScanCode_UNKNOWN,
    /*0xe004*/ LiteScanCode_UNKNOWN,
    /*0xe005*/ LiteScanCode_UNKNOWN,
    /*0xe006*/ LiteScanCode_UNKNOWN,
    /*0xe007*/ LiteScanCode_UNKNOWN,
    /*0xe008*/ LiteScanCode_UNKNOWN,
    /*0xe009*/ LiteScanCode_UNKNOWN,
    /*0xe00a*/ LiteScanCode_UNKNOWN,
    /*0xe00b*/ LiteScanCode_UNKNOWN,
    /*0xe00c*/ LiteScanCode_UNKNOWN,
    /*0xe00d*/ LiteScanCode_UNKNOWN,
    /*0xe00e*/ LiteScanCode_UNKNOWN,
    /*0xe00f*/ LiteScanCode_UNKNOWN,
    /*0xe010*/ LiteScanCode_AUDIOPREV,
    /*0xe011*/ LiteScanCode_UNKNOWN,
    /*0xe012*/ LiteScanCode_UNKNOWN,
    /*0xe013*/ LiteScanCode_UNKNOWN,
    /*0xe014*/ LiteScanCode_UNKNOWN,
    /*0xe015*/ LiteScanCode_UNKNOWN,
    /*0xe016*/ LiteScanCode_UNKNOWN,
    /*0xe017*/ LiteScanCode_UNKNOWN,
    /*0xe018*/ LiteScanCode_UNKNOWN,
    /*0xe019*/ LiteScanCode_AUDIONEXT,
    /*0xe01a*/ LiteScanCode_UNKNOWN,
    /*0xe01b*/ LiteScanCode_UNKNOWN,
    /*0xe01c*/ LiteScanCode_KP_ENTER,
    /*0xe01d*/ LiteScanCode_RCTRL,
    /*0xe01e*/ LiteScanCode_UNKNOWN,
    /*0xe01f*/ LiteScanCode_UNKNOWN,
    /*0xe020*/ LiteScanCode_MUTE,
    /*0xe021*/ LiteScanCode_CALCULATOR,
    /*0xe022*/ LiteScanCode_AUDIOPLAY,
    /*0xe023*/ LiteScanCode_UNKNOWN,
    /*0xe024*/ LiteScanCode_AUDIOSTOP,
    /*0xe025*/ LiteScanCode_UNKNOWN,
    /*0xe026*/ LiteScanCode_UNKNOWN,
    /*0xe027*/ LiteScanCode_UNKNOWN,
    /*0xe028*/ LiteScanCode_UNKNOWN,
    /*0xe029*/ LiteScanCode_UNKNOWN,
    /*0xe02a*/ LiteScanCode_UNKNOWN,
    /*0xe02b*/ LiteScanCode_UNKNOWN,
    /*0xe02c*/ LiteScanCode_UNKNOWN,
    /*0xe02d*/ LiteScanCode_UNKNOWN,
    /*0xe02e*/ LiteScanCode_VOLUMEDOWN,
    /*0xe02f*/ LiteScanCode_UNKNOWN,
    /*0xe030*/ LiteScanCode_VOLUMEUP,
    /*0xe031*/ LiteScanCode_UNKNOWN,
    /*0xe032*/ LiteScanCode_AC_HOME,
    /*0xe033*/ LiteScanCode_UNKNOWN,
    /*0xe034*/ LiteScanCode_UNKNOWN,
    /*0xe035*/ LiteScanCode_KP_DIVIDE,
    /*0xe036*/ LiteScanCode_UNKNOWN,
    /*0xe037*/ LiteScanCode_PRINTSCREEN,
    /*0xe038*/ LiteScanCode_RALT,
    /*0xe039*/ LiteScanCode_UNKNOWN,
    /*0xe03a*/ LiteScanCode_UNKNOWN,
    /*0xe03b*/ LiteScanCode_UNKNOWN,
    /*0xe03c*/ LiteScanCode_UNKNOWN,
    /*0xe03d*/ LiteScanCode_UNKNOWN,
    /*0xe03e*/ LiteScanCode_UNKNOWN,
    /*0xe03f*/ LiteScanCode_UNKNOWN,
    /*0xe040*/ LiteScanCode_UNKNOWN,
    /*0xe041*/ LiteScanCode_UNKNOWN,
    /*0xe042*/ LiteScanCode_UNKNOWN,
    /*0xe043*/ LiteScanCode_UNKNOWN,
    /*0xe044*/ LiteScanCode_UNKNOWN,
    /*0xe045*/ LiteScanCode_NUMLOCKCLEAR,
    /*0xe046*/ LiteScanCode_PAUSE,
    /*0xe047*/ LiteScanCode_HOME,
    /*0xe048*/ LiteScanCode_UP,
    /*0xe049*/ LiteScanCode_PAGEUP,
    /*0xe04a*/ LiteScanCode_UNKNOWN,
    /*0xe04b*/ LiteScanCode_LEFT,
    /*0xe04c*/ LiteScanCode_UNKNOWN,
    /*0xe04d*/ LiteScanCode_RIGHT,
    /*0xe04e*/ LiteScanCode_UNKNOWN,
    /*0xe04f*/ LiteScanCode_END,
    /*0xe050*/ LiteScanCode_DOWN,
    /*0xe051*/ LiteScanCode_PAGEDOWN,
    /*0xe052*/ LiteScanCode_INSERT,
    /*0xe053*/ LiteScanCode_DELETE,
    /*0xe054*/ LiteScanCode_UNKNOWN,
    /*0xe055*/ LiteScanCode_UNKNOWN,
    /*0xe056*/ LiteScanCode_UNKNOWN,
    /*0xe057*/ LiteScanCode_UNKNOWN,
    /*0xe058*/ LiteScanCode_UNKNOWN,
    /*0xe059*/ LiteScanCode_UNKNOWN,
    /*0xe05a*/ LiteScanCode_UNKNOWN,
    /*0xe05b*/ LiteScanCode_LGUI,
    /*0xe05c*/ LiteScanCode_RGUI,
    /*0xe05d*/ LiteScanCode_APPLICATION,
    /*0xe05e*/ LiteScanCode_POWER,
    /*0xe05f*/ LiteScanCode_SLEEP,
    /*0xe060*/ LiteScanCode_UNKNOWN,
    /*0xe061*/ LiteScanCode_UNKNOWN,
    /*0xe062*/ LiteScanCode_UNKNOWN,
    /*0xe063*/ LiteScanCode_UNKNOWN,
    /*0xe064*/ LiteScanCode_UNKNOWN,
    /*0xe065*/ LiteScanCode_AC_SEARCH,
    /*0xe066*/ LiteScanCode_AC_BOOKMARKS,
    /*0xe067*/ LiteScanCode_AC_REFRESH,
    /*0xe068*/ LiteScanCode_AC_STOP,
    /*0xe069*/ LiteScanCode_AC_FORWARD,
    /*0xe06a*/ LiteScanCode_AC_BACK,
    /*0xe06b*/ LiteScanCode_COMPUTER,
    /*0xe06c*/ LiteScanCode_MAIL,
    /*0xe06d*/ LiteScanCode_MEDIASELECT,
    /*0xe06e*/ LiteScanCode_UNKNOWN,
    /*0xe06f*/ LiteScanCode_UNKNOWN,
    /*0xe070*/ LiteScanCode_UNKNOWN,
    /*0xe071*/ LiteScanCode_UNKNOWN,
    /*0xe072*/ LiteScanCode_UNKNOWN,
    /*0xe073*/ LiteScanCode_UNKNOWN,
    /*0xe074*/ LiteScanCode_UNKNOWN,
    /*0xe075*/ LiteScanCode_UNKNOWN,
    /*0xe076*/ LiteScanCode_UNKNOWN,
    /*0xe077*/ LiteScanCode_UNKNOWN,
    /*0xe078*/ LiteScanCode_UNKNOWN,
    /*0xe079*/ LiteScanCode_UNKNOWN,
    /*0xe07a*/ LiteScanCode_UNKNOWN,
    /*0xe07b*/ LiteScanCode_UNKNOWN,
    /*0xe07c*/ LiteScanCode_UNKNOWN,
    /*0xe07d*/ LiteScanCode_UNKNOWN,
    /*0xe07e*/ LiteScanCode_UNKNOWN,
    /*0xe07f*/ LiteScanCode_UNKNOWN
};


static void lite_push_event(LiteEvent event)
{
    s_events_queue[s_events_queue_tail] = event;
    s_events_queue_tail = (s_events_queue_tail + 1) % LITE_EVENT_QUEUE_SIZE;
}


static LiteEvent lite_pop_event(void)
{
    if (s_events_queue_head == s_events_queue_tail)
    {
        if (s_events_arena_temp.arena != nullptr)
        {
            lite_arena_end_temp(s_events_arena_temp);
            s_events_arena_temp.arena = nullptr;
        }

        return (LiteEvent){
            .type = LiteEventType_None
        };
    }

    LiteEvent event = s_events_queue[s_events_queue_head];
    s_events_queue_head = (s_events_queue_head + 1) % LITE_EVENT_QUEUE_SIZE;
    return event;
}


static LiteScanCode lite_windows_scancode_convert(LPARAM lParam, WPARAM wParam)
{
    LiteScanCode code;
    uint8_t index;
    uint16_t keyFlags = HIWORD(lParam);
    uint16_t scanCode = LOBYTE(keyFlags);

    /* On-Screen Keyboard can send wrong scan codes with high-order bit set (key break code).
     * Strip high-order bit. */
    scanCode &= ~0x80;

    if (scanCode != 0)
    {
        if ((keyFlags & KF_EXTENDED) == KF_EXTENDED)
        {
            scanCode = MAKEWORD(scanCode, 0xe0);
        }
        else if (scanCode == 0x45)
        {
            /* Pause */
            scanCode = 0xe046;
        }
    }
    else
    {
        uint16_t vkCode = LOWORD(wParam);

        /* Pause/Break key have a special scan code with 0xe1 prefix.
         * Use Pause scan code that is used in Win32. */
        if (scanCode == 0xe11d)
        {
            scanCode = 0xe046;
        }
    }

    /* Pack scan code into one byte to make the index. */
    index = LOBYTE(scanCode) | (HIBYTE(scanCode) ? 0x80 : 0x00);
    code = lite_win32_scancode_table[index];

    return code;
}


static char* lite_ucs4_to_utf8(uint32_t ch, char *dst)
{
    uint8_t *p = (uint8_t*)dst;
    if (ch <= 0x7F)
    {
        *p = (uint8_t)ch;
        ++dst;
    }
    else if (ch <= 0x7FF)
    {
        p[0] = 0xC0 | (uint8_t)((ch >> 6) & 0x1F);
        p[1] = 0x80 | (uint8_t)(ch & 0x3F);
        dst += 2;
    }
    else if (ch <= 0xFFFF)
    {
        p[0] = 0xE0 | (uint8_t)((ch >> 12) & 0x0F);
        p[1] = 0x80 | (uint8_t)((ch >> 6) & 0x3F);
        p[2] = 0x80 | (uint8_t)(ch & 0x3F);
        dst += 3;
    }
    else
    {
        p[0] = 0xF0 | (uint8_t)((ch >> 18) & 0x07);
        p[1] = 0x80 | (uint8_t)((ch >> 12) & 0x3F);
        p[2] = 0x80 | (uint8_t)((ch >> 6) & 0x3F);
        p[3] = 0x80 | (uint8_t)(ch & 0x3F);
        dst += 4;
    }
    return dst;
}


const char* lite_get_scan_code_name(LiteScanCode scancode)
{
    const char *name;
    if (((int)scancode) < LiteScanCode_UNKNOWN || scancode >= LITE_NUM_SCANCODES)
    {
        // SDL_InvalidParamError("scancode");
        return "";
    }

    name = lite_scancode_names[scancode];
    if (name) {
        return name;
    }

    return "";
}


static const char* lite_get_key_name_cstr(LiteKeyCode key_code)
{
    static char name[8];
    char *end;

    if (key_code & LITE_SCANCODE_MASK)
    {
        return lite_get_scan_code_name((LiteScanCode)(key_code & ~LITE_SCANCODE_MASK));
    }

    switch (key_code)
    {
    case LiteKeyCode_RETURN:
        return lite_get_scan_code_name(LiteScanCode_RETURN);
    case LiteKeyCode_ESCAPE:
        return lite_get_scan_code_name(LiteScanCode_ESCAPE);
    case LiteKeyCode_BACKSPACE:
        return lite_get_scan_code_name(LiteScanCode_BACKSPACE);
    case LiteKeyCode_TAB:
        return lite_get_scan_code_name(LiteScanCode_TAB);
    case LiteKeyCode_SPACE:
        return lite_get_scan_code_name(LiteScanCode_SPACE);
    case LiteKeyCode_DELETE:
        return lite_get_scan_code_name(LiteScanCode_DELETE);
    default:
        /* Unaccented letter keys on latin keyboards are normally
           labeled in upper case (and probably on others like Greek or
           Cyrillic too, so if you happen to know for sure, please
           adapt this). */
        if (key_code >= 'a' && key_code <= 'z') {
            key_code -= 32;
        }

        end = lite_ucs4_to_utf8((uint32_t)key_code, name);
        *end = '\0';
        return name;
    }
}


static LiteStringView lite_get_key_name(LiteKeyCode key_code)
{
    LiteStringView key_name = lite_string_temp(lite_get_key_name_cstr(key_code));
    char* p = (char*)key_name.buffer;
    while (*p)
    {
        *p = tolower(*p);
        p++;
    }
    return key_name;
}

// static LiteStringView lite_get_key_name(WORD scanCode, WORD extended)
// {
//     if (extended)
//     {
//         if (scanCode != 0x45)
//         {
//             scanCode |= 0xE000;
//         }
//     }
//     else
//     {
//         if (scanCode == 0x45)
//         {
//             scanCode = 0xE11D45;
//         }
//         else if (scanCode == 0x54)
//         {
//             scanCode = 0xE037;
//         }
//     }

//     LONG lParam = 0;
//     if (extended)
//     {
//         if (extended == 0xE11D00)
//         {
//             lParam = 0x45 << 16;
//         }
//         else
//         {
//             lParam = (0x100 | (scanCode & 0xff)) << 16;
//         }
//     }
//     else
//     {
//         lParam = scanCode << 16;

//         if (scanCode == 0x45)
//         {
//             lParam |= (0x1 << 24);
//         }
//     }

//     LiteArena* frame_arena = s_events_arena;
//     const uint32_t length = 16;
//     char* text = (char*)lite_arena_acquire(frame_arena, length);
//     int text_length = GetKeyNameTextA(lParam, text, length);
//     for (int i = 0; i < text_length; i++)
//     {
//         text[i] = tolower(text[i]);
//     }

//     return lite_string_view(text, (uint32_t)text_length, 0);
// }

// -----------------------------------------------------------
// Handle mouse state
// -----------------------------------------------------------

static void lite_send_mouse_button_event(uint64_t timestamp, uint32_t mouse_id, LiteEventAction action, LiteMouseButton button)
{
    LiteEventType type;
    switch (action)
    {
        case LiteEventAction_None:
            return;

        case LiteEventAction_Pressed:
            type = LiteEventType_MouseDown;
            break;

        case LiteEventAction_Released:
            type = LiteEventType_MouseUp;
            break;
    }

    const LiteStringView button_names[] = {
        lite_string_lit("?"),
        lite_string_lit("left"),
        lite_string_lit("middle"),
        lite_string_lit("right"),
        lite_string_lit("x1"),
        lite_string_lit("x2"),
    };
    const LiteStringView button_name = button_names[(uint32_t)button];

    int32_t x, y;
    uint32_t mouse_flags = lite_get_mouse_state(&x, &y);

    if (action == LiteEventAction_Pressed)
    {
        mouse_flags |= LITE_MOUSE_BUTTON(button);
    }
    else
    {
        mouse_flags &= ~LITE_MOUSE_BUTTON(button);
    }
    lite_set_mouse_state(mouse_flags, x, y);


    int32_t clicks;
    LiteMouseButtonState* state = lite_get_mouse_button_state(button);
    if (state)
    {
        if (action == LiteEventAction_Pressed)
        {
            const uint64_t  double_click_time   = 200; // miliseconds
            const int32_t   double_click_radius = 32;

            const uint64_t now = (uint64_t)GetTickCount64(); // miliseconds
            if (now >= state->last_timestamp + double_click_time
                || (x - state->last_x) > double_click_radius
                || (y - state->last_y) > double_click_radius)
            {
                state->clicks = 0;
            }

            state->clicks += 1;
            state->last_x = x;
            state->last_y = y;
            state->last_timestamp = now;
        }

        clicks = state->clicks;
    }
    else
    {
        clicks = 1;
    }

    lite_push_event((LiteEvent){
        .type = type,
        .mouse_up = {
            .button_name = button_name,
            .x = x,
            .y = y,
            .clicks = clicks
        }
    });
}


static void lite_check_wparam_mouse_button(uint64_t timestamp, bool wparam_mouse_pressed, uint32_t mouse_flags, bool bSwapButtons, LiteMouseButton button, uint32_t mouse_id)
{
    if (bSwapButtons)
    {
        if (button == LiteMouseButton_Left)
        {
            button = LiteMouseButton_Right;
        }
        else if (button == LiteMouseButton_Right)
        {
            button = LiteMouseButton_Left;
        }
    }

    // if (data->focus_click_pending & LITE_MOUSE_BUTTON(button)) {
    //     /* Ignore the button click for activation */
    //     if (!bwParamMousePressed) {
    //         data->focus_click_pending &= ~LITE_MOUSE_BUTTON(button);
    //         WIN_UpdateClipCursor(data->window);
    //     }
    //     if (WIN_ShouldIgnoreFocusClick(data)) {
    //         return;
    //     }
    // }

    if (wparam_mouse_pressed && !(mouse_flags & LITE_MOUSE_BUTTON(button)))
    {
        lite_send_mouse_button_event(timestamp, mouse_id, LiteEventAction_Pressed, button);
    }
    else if (!wparam_mouse_pressed && (mouse_flags & LITE_MOUSE_BUTTON(button)))
    {
        lite_send_mouse_button_event(timestamp, mouse_id, LiteEventAction_Released, button);
    }
}


/*
 * Some windows systems fail to send a WM_LBUTTONDOWN sometimes, but each mouse move contains the current button state also
 *  so this function reconciles our view of the world with the current buttons reported by windows
 */
static void lite_check_wparam_mouse_buttons(uint64_t timestamp, WPARAM wParam, uint32_t mouse_id)
{
    uint32_t mouse_flags = lite_get_mouse_state(nullptr, nullptr);

    /* WM_LBUTTONDOWN and friends handle button swapping for us. No need to check SM_SWAPBUTTON here.  */
    lite_check_wparam_mouse_button(timestamp, (wParam & MK_LBUTTON), mouse_flags, false, LiteMouseButton_Left, mouse_id);
    lite_check_wparam_mouse_button(timestamp, (wParam & MK_MBUTTON), mouse_flags, false, LiteMouseButton_Middle, mouse_id);
    lite_check_wparam_mouse_button(timestamp, (wParam & MK_RBUTTON), mouse_flags, false, LiteMouseButton_Right, mouse_id);
    lite_check_wparam_mouse_button(timestamp, (wParam & MK_XBUTTON1), mouse_flags, false, LiteMouseButton_X1, mouse_id);
    lite_check_wparam_mouse_button(timestamp, (wParam & MK_XBUTTON2), mouse_flags, false, LiteMouseButton_X2, mouse_id);
}


// -----------------------------------------------------------
// Window Procedure
// -----------------------------------------------------------


static LRESULT WINAPI lite_win32_window_proc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    static int32_t prev_mouse_x = -1;
    static int32_t prev_mouse_y = -1;

    if (s_events_arena == nullptr)
    {
        s_events_arena = lite_arena_create(1024, 1024 * 1024, 1);
    }

    if (s_events_arena_temp.arena == nullptr)
    {
        s_events_arena_temp = lite_arena_begin_temp(s_events_arena);
    }

    LiteArena* frame_arena = s_events_arena;

    switch (msg)
    {
    case WM_SIZE:
    {
        //ReleaseDC(s_window, s_hSurface);
        UINT width = LOWORD(lParam);
        UINT height = HIWORD(lParam);

        DeleteBitmap(s_hSurfaceBitmap);
        DeleteDC(s_hSurface);

        s_surface_width = (int32_t)width;
        s_surface_height = (int32_t)height;

        s_hSurface = CreateCompatibleDC(s_hDC);

        BITMAPINFO bmi = { 0 };
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = (LONG)width;
        bmi.bmiHeader.biHeight = -(LONG)height; // top-down
        bmi.bmiHeader.biSizeImage = (DWORD)(s_surface_width * s_surface_height * sizeof(LiteColor));
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        s_hSurfaceBitmap = CreateDIBSection(s_hDC, &bmi, DIB_RGB_COLORS, (void**)(&s_surface_pixels), nullptr, 0);
        SelectObject(s_hSurface, s_hSurfaceBitmap);

        lite_push_event((LiteEvent){
            .type = LiteEventType_Resized,
            .resized.width = (int32_t)width,
            .resized.height = (int32_t)height,
        });
        return 0;
    }

    //case WM_EXPOSED:
    //    return 0;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        lite_push_event((LiteEvent){ .type = LiteEventType_Quit });
        PostQuitMessage(0);
        return 0;

    case WM_SETFOCUS:
        break;

    case WM_QUIT:
        lite_push_event((LiteEvent){ .type = LiteEventType_Quit });
        return 0;

    case WM_DROPFILES:
        break;

    case WM_SYSKEYUP:
    case WM_KEYUP:
    {
        LiteScanCode scan_code = lite_windows_scancode_convert(lParam, wParam);
        LiteKeyCode key_code = lite_default_keymap[scan_code];

        lite_push_event((LiteEvent){
            .type = LiteEventType_KeyUp,
            .key_up.key_name = lite_get_key_name(key_code),
        });
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
    {
        LiteScanCode scan_code = lite_windows_scancode_convert(lParam, wParam);
        LiteKeyCode key_code = lite_default_keymap[scan_code];

        lite_push_event((LiteEvent){
            .type = LiteEventType_KeyDown,
            .key_down.key_name = lite_get_key_name(key_code),
        });
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }

    case WM_CHAR:
    {
        static WCHAR high_surrogate = L'\0';

        const bool is_high_surrogate = (((wParam) >= 0xd800) && ((wParam) <= 0xdbff));
        if (is_high_surrogate)
        {
            high_surrogate = (WCHAR)wParam;
        }
        else
        {
            char utf8[5];
            WCHAR utf16[] = {
                high_surrogate ? high_surrogate : (WCHAR)wParam,
                high_surrogate ? (WCHAR)wParam : L'\0',
                L'\0'
            };

            if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, utf16, -1, utf8, sizeof(utf8), nullptr, nullptr) > 0)
            {
                // Don't post text events for unprintable characters
                if (iscntrl((unsigned char)*utf8))
                {
                    return 0;
                }

                const uint32_t length = lite_string_count(utf8);
                char* text = (char*)lite_arena_acquire(frame_arena, length + 1);
                memcpy(text, utf8, length);

                lite_push_event((LiteEvent){
                    .type = LiteEventType_TextInput,
                    .text_input.text = lite_string_view(text, length, 0),
                });
            }

            high_surrogate = L'\0';
        }

        return 0;
    }

    case WM_UNICHAR:
    {
        if (wParam == UNICODE_NOCHAR)
        {
            return 1;
        }

        const uint32_t length = 4;
        char* text = (char*)lite_arena_acquire(frame_arena, length + 1);

        uint32_t ch = (uint32_t)wParam;
        uint8_t *p = (uint8_t*)text;
        if (ch <= 0x7F)
        {
            *p = (uint8_t)ch;
        }
        else if (ch <= 0x7FF)
        {
            p[0] = 0xC0 | (uint8_t)((ch >> 6) & 0x1F);
            p[1] = 0x80 | (uint8_t)(ch & 0x3F);
            p[2] = 0;
        }
        else if (ch <= 0xFFFF)
        {
            p[0] = 0xE0 | (uint8_t)((ch >> 12) & 0x0F);
            p[1] = 0x80 | (uint8_t)((ch >> 6) & 0x3F);
            p[2] = 0x80 | (uint8_t)(ch & 0x3F);
            p[3] = 0;
        }
        else
        {
            p[0] = 0xF0 | (uint8_t)((ch >> 18) & 0x07);
            p[1] = 0x80 | (uint8_t)((ch >> 12) & 0x3F);
            p[2] = 0x80 | (uint8_t)((ch >> 6) & 0x3F);
            p[3] = 0x80 | (uint8_t)(ch & 0x3F);
            p[4] = 0;
        }

        lite_push_event((LiteEvent){
            .type = LiteEventType_TextInput,
            .text_input.text = lite_string_view(text, length, 0),
        });
        return 0;
    }

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    {
        lite_check_wparam_mouse_buttons((uint32_t)GetTickCount64(), wParam, 0);
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        LiteEvent event;
        event.type = LiteEventType_MouseMove;
        event.mouse_move.x = (int32_t)x;
        event.mouse_move.y = (int32_t)y;

        if (prev_mouse_x < 0)
        {
            event.mouse_move.dx = 0;
        }
        else
        {
            event.mouse_move.dx = event.mouse_move.x - prev_mouse_x;
        }

        if (prev_mouse_y < 0)
        {
            event.mouse_move.dy = 0;
        }
        else
        {
            event.mouse_move.dy = event.mouse_move.y - prev_mouse_y;
        }

        prev_mouse_x = x;
        prev_mouse_y = y;

        lite_push_event(event);
        lite_set_mouse_position(x, y);
        return 0;
    }

    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
    {
        short amount = GET_WHEEL_DELTA_WPARAM(wParam);
        float fAmount = (float)amount / WHEEL_DELTA;

        int32_t x = 0;
        int32_t y = 0;
        if (msg == WM_MOUSEWHEEL)
        {
            y = (int32_t)fAmount;
        }
        else
        {
            x = (int32_t)fAmount;
        }

        lite_push_event((LiteEvent){
            .type = LiteEventType_MouseWheel,
            .mouse_wheel.x = x,
            .mouse_wheel.y = y,
        });
    } break;

    default:
        break;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}


void lite_sleep(uint64_t ms)
{
    Sleep((DWORD)ms);
}


void lite_usleep(uint64_t us)
{
    /* 'NTSTATUS NTAPI NtDelayExecution(BOOL Alerted, PLARGE_INTEGER time);' */
    /* 'typedef LONG NTSTATUS;' =)) */
    /* '#define NTAPI __stdcall' =)) */
    typedef LONG(__stdcall* NtDelayExecutionFN)(BOOL, PLARGE_INTEGER);

    static bool done_finding;
    static NtDelayExecutionFN NtDelayExecution;

    if (!NtDelayExecution && !done_finding)
    {
        done_finding = 1;
        HMODULE module = GetModuleHandleA("ntdll.dll");
        const char* func = "NtDelayExecution";
        NtDelayExecution = (NtDelayExecutionFN)GetProcAddress(module, func);
    }

    if (NtDelayExecution)
    {
        LARGE_INTEGER times;
        times.QuadPart = (LONGLONG)(-us * 10); // Timer is precise as 100 nanoseconds
        (void)NtDelayExecution(FALSE, &times);
    }
    else
    {
        Sleep((DWORD)(us / 1000));
    }
}


uint64_t lite_cpu_ticks(void)
{
    LARGE_INTEGER liCounter;
    QueryPerformanceCounter(&liCounter);
    return (uint64_t)liCounter.QuadPart;
}


uint64_t lite_cpu_frequency(void)
{
    LARGE_INTEGER liFrequency;
    QueryPerformanceFrequency(&liFrequency);
    return (uint64_t)liFrequency.QuadPart;
}


LiteStringView lite_clipboard_get(void)
{
    if (!IsClipboardFormatAvailable(CF_TEXT))
    {
        return lite_string_lit("");
    }

    if (!OpenClipboard(s_window))
    {
        return lite_string_lit("");
    }

    LiteStringView result = lite_string_lit("");
    HGLOBAL hGlobal = GetClipboardData(CF_TEXT);
    if (hGlobal != nullptr)
    {
        const char* lpStr = (const char*)GlobalLock(hGlobal);
        if (lpStr != nullptr)
        {
            result.buffer = lpStr;
            result.length = lite_string_count(lpStr);
            GlobalUnlock(hGlobal);
        }
    }

    CloseClipboard();
    return result;
}


bool lite_clipboard_set(LiteStringView text)
{
    if (!OpenClipboard(s_window))
    {
        return false;
    }

    HGLOBAL hGlobalCopy = GlobalAlloc(GMEM_MOVEABLE, text.length + 1);
    if (hGlobalCopy == nullptr)
    {
        CloseClipboard();
        return false;
    }

    void* lpStr = GlobalLock(hGlobalCopy);
    memcpy(lpStr, text.buffer, text.length + 1);

    bool result = SetClipboardData(CF_TEXT, hGlobalCopy) != nullptr;

    GlobalUnlock(hGlobalCopy);
    CloseClipboard();

    return result;
}


void lite_console_open(void)
{
#if defined(_WIN32)
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif
}


void lite_console_close(void)
{
#if defined(_WIN32)
    FreeConsole();
#endif
}


void lite_window_open(void)
{
    SetProcessDPIAware();
    SetThreadExecutionState(ES_CONTINUOUS);

    // @maihd(maihd): convert this to win32
    //SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    //atexit(SDL_Quit);

    // Acquire focus when mouse click
    // @todo(maihd): convert this to win32
    //#if SDL_VERSION_ATLEAST(2, 0, 5)
    //SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
    //#endif

    // Get current display monitor size
#if 0
//     HMONITOR hMonitor = ;
//     MONITORINFO mi;

//     SDL_DisplayMode dm;
//     SDL_GetCurrentDisplayMode(0, &dm);
#else
    // Use old-style function to get monitor size
    DWORD monitor_width = GetSystemMetrics(SM_CXSCREEN);
    DWORD monitor_height = GetSystemMetrics(SM_CYSCREEN);
#endif

    const char* window_class = "lite_window_class";
    WNDCLASSA wc = {0};
    wc.lpfnWndProc    = lite_win32_window_proc;
    wc.hInstance      = GetModuleHandle(nullptr);
    wc.lpszClassName  = window_class;
    if (!RegisterClassA(&wc))
    {
        assert(0);
        return;
    }

    DWORD window_titlebar_size = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME);
    DWORD window_border_size = GetSystemMetrics(SM_CXFRAME);

    DWORD window_width = (DWORD)((float)monitor_width * 0.8f) + window_border_size;
    DWORD window_height = (DWORD)((float)monitor_height * 0.8f) + window_titlebar_size;

    DWORD window_x = (monitor_width - window_width) / 2;
    DWORD window_y = (monitor_height - window_height) / 2;

    s_window = CreateWindowA(
        window_class,
        "",
        //WS_POPUP,
        WS_OVERLAPPEDWINDOW,
        window_x, window_y,
        window_width, window_height,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr
    );
    if (s_window == nullptr)
    {
        // @todo(maihd): handle error
        return;
    }

    s_hDC = GetDC(s_window);

    ShowWindow(s_window, true);
}


void lite_window_close(void)
{
    DeleteBitmap(s_hSurfaceBitmap);
    DeleteDC(s_hSurface);

    ReleaseDC(s_window, s_hDC);
    DestroyWindow(s_window);
    s_window = nullptr;
}


void* lite_window_handle(void)
{
    return (void*)(uintptr_t)s_window;
}


void* lite_window_surface(int32_t* width, int32_t* height)
{
    *width = s_surface_width;
    *height = s_surface_height;
    return s_surface_pixels;
}


void lite_window_show(void)
{
    ShowWindow(s_window, true);
}


void lite_window_hide(void)
{
    ShowWindow(s_window, false);
}


void lite_window_show_titlebar(void)
{
    SetWindowLongA(s_window, GWL_STYLE, WS_OVERLAPPEDWINDOW);
}


void lite_window_hide_titlebar(void)
{
    SetWindowLongA(s_window, GWL_STYLE, WS_POPUP);
}


void lite_window_set_mode(LiteWindowMode mode)
{
    switch (mode)
    {
    case LiteWindowMode_Normal:
        PostMessageA(s_window, WM_SYSCOMMAND, SC_RESTORE, 0);
        break;

    case LiteWindowMode_Maximized:
        PostMessageA(s_window, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        break;

    case LiteWindowMode_FullScreen:
        break;
    }
}


void lite_window_set_title(const char* title)
{
    SetWindowTextA(s_window, title);
}


void lite_window_set_cursor(LiteCursor cursor)
{
    static LPCSTR cursor_names[LiteCursor_COUNT] = {
        nullptr,
        IDC_HAND,
        IDC_ARROW,
        IDC_IBEAM,
        IDC_SIZEWE,
        IDC_SIZENS,
    };
    static HCURSOR cursor_caches[LiteCursor_COUNT];

    HCURSOR hCursor = cursor_caches[cursor];
    if (hCursor == nullptr)
    {
        hCursor = LoadCursorA(nullptr, cursor_names[cursor]);
        cursor_caches[cursor] = hCursor;
    }
    SetCursor(hCursor);
}


float lite_window_dpi(void)
{
    return (float)GetDpiForWindow(s_window);
}


bool lite_window_has_focus(void)
{
    bool result = GetFocus() == s_window;
    return result;
}


void lite_window_update_rects(struct LiteRect* rects, uint32_t count)
{
    // Update pixels per rects
    // for (uint32_t i = 0; i < count; i++)
    // {
    //    const LiteRect rect = rects[i];
    //    BitBlt(s_hDC, rect.x, rect.y, rect.width, rect.height, s_hSurface, rect.x, rect.y, SRCCOPY);
    // }

    // Copy all pixels surface to s_window
    BitBlt(s_hDC, 0, 0, s_surface_width, s_surface_height, s_hSurface, 0, 0, SRCCOPY);
}


void lite_window_message_box(const char* title, const char* message)
{
    MessageBoxA(s_window, message, title, MB_OK);
}


bool lite_window_confirm_dialog(const char* title, const char* message)
{
    return MessageBoxA(s_window, message, title, MB_YESNO) == IDYES;
}


LiteEvent lite_window_poll_event(void)
{
    MSG msg;
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return lite_pop_event();
}


bool lite_window_wait_event(uint64_t time_us)
{
    MSG msg;
    uint64_t time_remain_us = time_us;
    uint64_t time_step_us = 1000 * 1000 / 60;
    while (!PeekMessageA(&msg, nullptr, 0, 0, PM_NOREMOVE))
    {
        if (time_remain_us == 0)
        {
            return false;
        }
        else if (time_step_us > time_remain_us)
        {
            lite_usleep(time_remain_us);
            time_remain_us = 0;
        }
        else
        {
            lite_usleep(time_step_us);
            time_remain_us -= time_step_us;
        }
    }

    return true;
}

//! EOF

