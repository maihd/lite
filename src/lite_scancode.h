#pragma once

/**
 *  The SDL keyboard scancode representation.
 *
 *  Values of this type are used to represent keyboard keys, among other places
 *  in the \link SDL_Keysym::scancode key.keysym.scancode \endlink field of the
 *  SDL_Event structure.
 *
 *  The values in this enumeration are based on the USB usage page standard:
 *  https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
 */
typedef enum
{
    LiteScanCode_UNKNOWN = 0,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
    /* @{ */

    LiteScanCode_A = 4,
    LiteScanCode_B = 5,
    LiteScanCode_C = 6,
    LiteScanCode_D = 7,
    LiteScanCode_E = 8,
    LiteScanCode_F = 9,
    LiteScanCode_G = 10,
    LiteScanCode_H = 11,
    LiteScanCode_I = 12,
    LiteScanCode_J = 13,
    LiteScanCode_K = 14,
    LiteScanCode_L = 15,
    LiteScanCode_M = 16,
    LiteScanCode_N = 17,
    LiteScanCode_O = 18,
    LiteScanCode_P = 19,
    LiteScanCode_Q = 20,
    LiteScanCode_R = 21,
    LiteScanCode_S = 22,
    LiteScanCode_T = 23,
    LiteScanCode_U = 24,
    LiteScanCode_V = 25,
    LiteScanCode_W = 26,
    LiteScanCode_X = 27,
    LiteScanCode_Y = 28,
    LiteScanCode_Z = 29,

    LiteScanCode_1 = 30,
    LiteScanCode_2 = 31,
    LiteScanCode_3 = 32,
    LiteScanCode_4 = 33,
    LiteScanCode_5 = 34,
    LiteScanCode_6 = 35,
    LiteScanCode_7 = 36,
    LiteScanCode_8 = 37,
    LiteScanCode_9 = 38,
    LiteScanCode_0 = 39,

    LiteScanCode_RETURN = 40,
    LiteScanCode_ESCAPE = 41,
    LiteScanCode_BACKSPACE = 42,
    LiteScanCode_TAB = 43,
    LiteScanCode_SPACE = 44,

    LiteScanCode_MINUS = 45,
    LiteScanCode_EQUALS = 46,
    LiteScanCode_LEFTBRACKET = 47,
    LiteScanCode_RIGHTBRACKET = 48,
    LiteScanCode_BACKSLASH = 49, /**< Located at the lower left of the return
                                  *   key on ISO keyboards and at the right end
                                  *   of the QWERTY row on ANSI keyboards.
                                  *   Produces REVERSE SOLIDUS (backslash) and
                                  *   VERTICAL LINE in a US layout, REVERSE
                                  *   SOLIDUS and VERTICAL LINE in a UK Mac
                                  *   layout, NUMBER SIGN and TILDE in a UK
                                  *   Windows layout, DOLLAR SIGN and POUND SIGN
                                  *   in a Swiss German layout, NUMBER SIGN and
                                  *   APOSTROPHE in a German layout, GRAVE
                                  *   ACCENT and POUND SIGN in a French Mac
                                  *   layout, and ASTERISK and MICRO SIGN in a
                                  *   French Windows layout.
                                  */
    LiteScanCode_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                                  *   instead of 49 for the same key, but all
                                  *   OSes I've seen treat the two codes
                                  *   identically. So, as an implementor, unless
                                  *   your keyboard generates both of those
                                  *   codes and your OS treats them differently,
                                  *   you should generate LiteScanCode_BACKSLASH
                                  *   instead of this code. As a user, you
                                  *   should not rely on this code because SDL
                                  *   will never generate it with most (all?)
                                  *   keyboards.
                                  */
    LiteScanCode_SEMICOLON = 51,
    LiteScanCode_APOSTROPHE = 52,
    LiteScanCode_GRAVE = 53, /**< Located in the top left corner (on both ANSI
                              *   and ISO keyboards). Produces GRAVE ACCENT and
                              *   TILDE in a US Windows layout and in US and UK
                              *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                              *   and NOT SIGN in a UK Windows layout, SECTION
                              *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                              *   layouts on ISO keyboards, SECTION SIGN and
                              *   DEGREE SIGN in a Swiss German layout (Mac:
                              *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                              *   DEGREE SIGN in a German layout (Mac: only on
                              *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                              *   French Windows layout, COMMERCIAL AT and
                              *   NUMBER SIGN in a French Mac layout on ISO
                              *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                              *   SIGN in a Swiss German, German, or French Mac
                              *   layout on ANSI keyboards.
                              */
    LiteScanCode_COMMA = 54,
    LiteScanCode_PERIOD = 55,
    LiteScanCode_SLASH = 56,

    LiteScanCode_CAPSLOCK = 57,

    LiteScanCode_F1 = 58,
    LiteScanCode_F2 = 59,
    LiteScanCode_F3 = 60,
    LiteScanCode_F4 = 61,
    LiteScanCode_F5 = 62,
    LiteScanCode_F6 = 63,
    LiteScanCode_F7 = 64,
    LiteScanCode_F8 = 65,
    LiteScanCode_F9 = 66,
    LiteScanCode_F10 = 67,
    LiteScanCode_F11 = 68,
    LiteScanCode_F12 = 69,

    LiteScanCode_PRINTSCREEN = 70,
    LiteScanCode_SCROLLLOCK = 71,
    LiteScanCode_PAUSE = 72,
    LiteScanCode_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                   does send code 73, not 117) */
    LiteScanCode_HOME = 74,
    LiteScanCode_PAGEUP = 75,
    LiteScanCode_DELETE = 76,
    LiteScanCode_END = 77,
    LiteScanCode_PAGEDOWN = 78,
    LiteScanCode_RIGHT = 79,
    LiteScanCode_LEFT = 80,
    LiteScanCode_DOWN = 81,
    LiteScanCode_UP = 82,

    LiteScanCode_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                                     */
    LiteScanCode_KP_DIVIDE = 84,
    LiteScanCode_KP_MULTIPLY = 85,
    LiteScanCode_KP_MINUS = 86,
    LiteScanCode_KP_PLUS = 87,
    LiteScanCode_KP_ENTER = 88,
    LiteScanCode_KP_1 = 89,
    LiteScanCode_KP_2 = 90,
    LiteScanCode_KP_3 = 91,
    LiteScanCode_KP_4 = 92,
    LiteScanCode_KP_5 = 93,
    LiteScanCode_KP_6 = 94,
    LiteScanCode_KP_7 = 95,
    LiteScanCode_KP_8 = 96,
    LiteScanCode_KP_9 = 97,
    LiteScanCode_KP_0 = 98,
    LiteScanCode_KP_PERIOD = 99,

    LiteScanCode_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                        *   keyboards have over ANSI ones,
                                        *   located between left shift and Y.
                                        *   Produces GRAVE ACCENT and TILDE in a
                                        *   US or UK Mac layout, REVERSE SOLIDUS
                                        *   (backslash) and VERTICAL LINE in a
                                        *   US or UK Windows layout, and
                                        *   LESS-THAN SIGN and GREATER-THAN SIGN
                                        *   in a Swiss German, German, or French
                                        *   layout. */
    LiteScanCode_APPLICATION = 101, /**< windows contextual menu, compose */
    LiteScanCode_POWER = 102, /**< The USB document says this is a status flag,
                               *   not a physical key - but some Mac keyboards
                               *   do have a power key. */
    LiteScanCode_KP_EQUALS = 103,
    LiteScanCode_F13 = 104,
    LiteScanCode_F14 = 105,
    LiteScanCode_F15 = 106,
    LiteScanCode_F16 = 107,
    LiteScanCode_F17 = 108,
    LiteScanCode_F18 = 109,
    LiteScanCode_F19 = 110,
    LiteScanCode_F20 = 111,
    LiteScanCode_F21 = 112,
    LiteScanCode_F22 = 113,
    LiteScanCode_F23 = 114,
    LiteScanCode_F24 = 115,
    LiteScanCode_EXECUTE = 116,
    LiteScanCode_HELP = 117,    /**< AL Integrated Help Center */
    LiteScanCode_MENU = 118,    /**< Menu (show menu) */
    LiteScanCode_SELECT = 119,
    LiteScanCode_STOP = 120,    /**< AC Stop */
    LiteScanCode_AGAIN = 121,   /**< AC Redo/Repeat */
    LiteScanCode_UNDO = 122,    /**< AC Undo */
    LiteScanCode_CUT = 123,     /**< AC Cut */
    LiteScanCode_COPY = 124,    /**< AC Copy */
    LiteScanCode_PASTE = 125,   /**< AC Paste */
    LiteScanCode_FIND = 126,    /**< AC Find */
    LiteScanCode_MUTE = 127,
    LiteScanCode_VOLUMEUP = 128,
    LiteScanCode_VOLUMEDOWN = 129,
/* not sure whether there's a reason to enable these */
/*     LiteScanCode_LOCKINGCAPSLOCK = 130,  */
/*     LiteScanCode_LOCKINGNUMLOCK = 131, */
/*     LiteScanCode_LOCKINGSCROLLLOCK = 132, */
    LiteScanCode_KP_COMMA = 133,
    LiteScanCode_KP_EQUALSAS400 = 134,

    LiteScanCode_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                            footnotes in USB doc */
    LiteScanCode_INTERNATIONAL2 = 136,
    LiteScanCode_INTERNATIONAL3 = 137, /**< Yen */
    LiteScanCode_INTERNATIONAL4 = 138,
    LiteScanCode_INTERNATIONAL5 = 139,
    LiteScanCode_INTERNATIONAL6 = 140,
    LiteScanCode_INTERNATIONAL7 = 141,
    LiteScanCode_INTERNATIONAL8 = 142,
    LiteScanCode_INTERNATIONAL9 = 143,
    LiteScanCode_LANG1 = 144, /**< Hangul/English toggle */
    LiteScanCode_LANG2 = 145, /**< Hanja conversion */
    LiteScanCode_LANG3 = 146, /**< Katakana */
    LiteScanCode_LANG4 = 147, /**< Hiragana */
    LiteScanCode_LANG5 = 148, /**< Zenkaku/Hankaku */
    LiteScanCode_LANG6 = 149, /**< reserved */
    LiteScanCode_LANG7 = 150, /**< reserved */
    LiteScanCode_LANG8 = 151, /**< reserved */
    LiteScanCode_LANG9 = 152, /**< reserved */

    LiteScanCode_ALTERASE = 153,    /**< Erase-Eaze */
    LiteScanCode_SYSREQ = 154,
    LiteScanCode_CANCEL = 155,      /**< AC Cancel */
    LiteScanCode_CLEAR = 156,
    LiteScanCode_PRIOR = 157,
    LiteScanCode_RETURN2 = 158,
    LiteScanCode_SEPARATOR = 159,
    LiteScanCode_OUT = 160,
    LiteScanCode_OPER = 161,
    LiteScanCode_CLEARAGAIN = 162,
    LiteScanCode_CRSEL = 163,
    LiteScanCode_EXSEL = 164,

    LiteScanCode_KP_00 = 176,
    LiteScanCode_KP_000 = 177,
    LiteScanCode_THOUSANDSSEPARATOR = 178,
    LiteScanCode_DECIMALSEPARATOR = 179,
    LiteScanCode_CURRENCYUNIT = 180,
    LiteScanCode_CURRENCYSUBUNIT = 181,
    LiteScanCode_KP_LEFTPAREN = 182,
    LiteScanCode_KP_RIGHTPAREN = 183,
    LiteScanCode_KP_LEFTBRACE = 184,
    LiteScanCode_KP_RIGHTBRACE = 185,
    LiteScanCode_KP_TAB = 186,
    LiteScanCode_KP_BACKSPACE = 187,
    LiteScanCode_KP_A = 188,
    LiteScanCode_KP_B = 189,
    LiteScanCode_KP_C = 190,
    LiteScanCode_KP_D = 191,
    LiteScanCode_KP_E = 192,
    LiteScanCode_KP_F = 193,
    LiteScanCode_KP_XOR = 194,
    LiteScanCode_KP_POWER = 195,
    LiteScanCode_KP_PERCENT = 196,
    LiteScanCode_KP_LESS = 197,
    LiteScanCode_KP_GREATER = 198,
    LiteScanCode_KP_AMPERSAND = 199,
    LiteScanCode_KP_DBLAMPERSAND = 200,
    LiteScanCode_KP_VERTICALBAR = 201,
    LiteScanCode_KP_DBLVERTICALBAR = 202,
    LiteScanCode_KP_COLON = 203,
    LiteScanCode_KP_HASH = 204,
    LiteScanCode_KP_SPACE = 205,
    LiteScanCode_KP_AT = 206,
    LiteScanCode_KP_EXCLAM = 207,
    LiteScanCode_KP_MEMSTORE = 208,
    LiteScanCode_KP_MEMRECALL = 209,
    LiteScanCode_KP_MEMCLEAR = 210,
    LiteScanCode_KP_MEMADD = 211,
    LiteScanCode_KP_MEMSUBTRACT = 212,
    LiteScanCode_KP_MEMMULTIPLY = 213,
    LiteScanCode_KP_MEMDIVIDE = 214,
    LiteScanCode_KP_PLUSMINUS = 215,
    LiteScanCode_KP_CLEAR = 216,
    LiteScanCode_KP_CLEARENTRY = 217,
    LiteScanCode_KP_BINARY = 218,
    LiteScanCode_KP_OCTAL = 219,
    LiteScanCode_KP_DECIMAL = 220,
    LiteScanCode_KP_HEXADECIMAL = 221,

    LiteScanCode_LCTRL = 224,
    LiteScanCode_LSHIFT = 225,
    LiteScanCode_LALT = 226, /**< alt, option */
    LiteScanCode_LGUI = 227, /**< windows, command (apple), meta */
    LiteScanCode_RCTRL = 228,
    LiteScanCode_RSHIFT = 229,
    LiteScanCode_RALT = 230, /**< alt gr, option */
    LiteScanCode_RGUI = 231, /**< windows, command (apple), meta */

    LiteScanCode_MODE = 257,    /**< I'm not sure if this is really not covered
                                 *   by any of the above, but since there's a
                                 *   special SDL_KMOD_MODE for it I'm adding it here
                                 */

    /* @} *//* Usage page 0x07 */

    /**
     *  \name Usage page 0x0C
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     *  See https://usb.org/sites/default/files/hut1_2.pdf
     *
     *  There are way more keys in the spec than we can represent in the
     *  current scancode range, so pick the ones that commonly come up in
     *  real world usage.
     */
    /* @{ */

    LiteScanCode_AUDIONEXT = 258,
    LiteScanCode_AUDIOPREV = 259,
    LiteScanCode_AUDIOSTOP = 260,
    LiteScanCode_AUDIOPLAY = 261,
    LiteScanCode_AUDIOMUTE = 262,
    LiteScanCode_MEDIASELECT = 263,
    LiteScanCode_WWW = 264,             /**< AL Internet Browser */
    LiteScanCode_MAIL = 265,
    LiteScanCode_CALCULATOR = 266,      /**< AL Calculator */
    LiteScanCode_COMPUTER = 267,
    LiteScanCode_AC_SEARCH = 268,       /**< AC Search */
    LiteScanCode_AC_HOME = 269,         /**< AC Home */
    LiteScanCode_AC_BACK = 270,         /**< AC Back */
    LiteScanCode_AC_FORWARD = 271,      /**< AC Forward */
    LiteScanCode_AC_STOP = 272,         /**< AC Stop */
    LiteScanCode_AC_REFRESH = 273,      /**< AC Refresh */
    LiteScanCode_AC_BOOKMARKS = 274,    /**< AC Bookmarks */

    /* @} *//* Usage page 0x0C */

    /**
     *  \name Walther keys
     *
     *  These are values that Christian Walther added (for mac keyboard?).
     */
    /* @{ */

    LiteScanCode_BRIGHTNESSDOWN = 275,
    LiteScanCode_BRIGHTNESSUP = 276,
    LiteScanCode_DISPLAYSWITCH = 277, /**< display mirroring/dual display
                                           switch, video mode switch */
    LiteScanCode_KBDILLUMTOGGLE = 278,
    LiteScanCode_KBDILLUMDOWN = 279,
    LiteScanCode_KBDILLUMUP = 280,
    LiteScanCode_EJECT = 281,
    LiteScanCode_SLEEP = 282,           /**< SC System Sleep */

    LiteScanCode_APP1 = 283,
    LiteScanCode_APP2 = 284,

    /* @} *//* Walther keys */

    /**
     *  \name Usage page 0x0C (additional media keys)
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
    /* @{ */

    LiteScanCode_AUDIOREWIND = 285,
    LiteScanCode_AUDIOFASTFORWARD = 286,

    /* @} *//* Usage page 0x0C (additional media keys) */

    /**
     *  \name Mobile keys
     *
     *  These are values that are often used on mobile phones.
     */
    /* @{ */

    LiteScanCode_SOFTLEFT = 287, /**< Usually situated below the display on phones and
                                      used as a multi-function feature key for selecting
                                      a software defined function shown on the bottom left
                                      of the display. */
    LiteScanCode_SOFTRIGHT = 288, /**< Usually situated below the display on phones and
                                       used as a multi-function feature key for selecting
                                       a software defined function shown on the bottom right
                                       of the display. */
    LiteScanCode_CALL = 289, /**< Used for accepting phone calls. */
    LiteScanCode_ENDCALL = 290, /**< Used for rejecting phone calls. */

    /* @} *//* Mobile keys */

    /* Add any other keys here. */

    LITE_NUM_SCANCODES = 512 /**< not a key, just marks the number of scancodes
                                 for array bounds */
} LiteScanCode;

//! EOF

