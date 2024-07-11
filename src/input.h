#ifndef _O_U16TEXT
#define _O_U16TEXT 0x20000
#endif

#define BIT_ESC224 0x80000000
#define BIT_ESC0 0x40000000

#define ESC_BITMASK 0x80000000
#define CTRLCHAR_BITMASK 0x40000000
#define FKEYS_BITMASK 0x20000000
#define TILDE_BITMASK 0x10000000

typedef struct InputUTF8
{
  utf8_int32_t utf8char;
  unsigned int flags;
  char equiv[100];
  size_t inputlen;
} InputUTF8;

enum CONTROL_CODES
#ifdef WIN32
{
  BS = 8,
  TAB = 9,
  CTRLENTER = 10,
  ENTER = 13,
  CTRLBS = 127,
  ESC = 27,

  UP = 72 | BIT_ESC224,
  LEFT = 75 | BIT_ESC224,
  RIGHT = 77 | BIT_ESC224,
  DOWN = 80 | BIT_ESC224,

  ALTUP = 152,
  ALTLEFT = 155,
  ALTRIGHT = 157,
  ALTDOWN = 160,

  HOME = 71,
  END = 79,

  CTRLHOME = 119,
  CTRLEND = 117,

  INS = 82,
  DEL = 83,

  CTRLA = 1,
  CTRLB = 2,
  CTRLC = 3,
  CTRLD = 4,
  CTRLE = 5,
  CTRLF = 6,
  CTRLG = 7,
  CTRLH = 8,
  CTRLI = 9,
  CTRLJ = 10,
  CTRLK = 11,
  CTRLL = 12,
  CTRLM = 13,
  CTRLN = 14,
  CTRLO = 15,
  CTRLP = 16,
  CTRLQ = 17,
  CTRLR = 18,
  CTRLS = 19,
  CTRLT = 20,
  CTRLU = 21,
  CTRLV = 22,
  CTRLW = 23,
  CTRLX = 24,
  CTRLY = 25,
  CTRLZ = 26,

  /* Different naming only happens in F11 and F12 */
  F1 = 59,
  F2 = 60,
  F3 = 61,
  F4 = 62,
  F5 = 63,
  F6 = 64,
  F7 = 65,
  F8 = 66,
  F9 = 67,
  F10 = 68,

  F11 = 133,
  F12 = 134,

  PGUP = 73,
  PGDW = 81,

  CTRLF1 = 94,
  CTRLF2 = 95,
  CTRLF3 = 96,
  CTRLF4 = 97,
  CTRLF5 = 98,
  CTRLF6 = 99,
  CTRLF7 = 100,
  CTRLF8 = 101,
  CTRLF9 = 102,
  CTRLF10 = 103,
  CTRLF11 = 137,
  CTRLF12 = 138,

  ALTF1 = 104,
  ALTF2 = 105,
  ALTF3 = 106,
  ALTF4 = 107,
  ALTF5 = 108,
  ALTF6 = 109,
  ALTF7 = 110,
  ALTF8 = 111,
  ALTF9 = 112,
  ALTF10 = 113,
  ALTF11 = 139,
  ALTF12 = 140,

  ALTINS = 162,
  ALTHOME = 151,
  ALTPGUP = 153,
  ALTDEL = 163,
  ALTEND = 159,
  ALTPGDW = 161,

  SHIFTF1 = 84,
  SHIFTF2 = 85,
  SHIFTF3 = 86,
  SHIFTF4 = 87,
  SHIFTF5 = 88,
  SHIFTF6 = 89,
  SHIFTF7 = 90,
  SHIFTF8 = 91,
  SHIFTF9 = 92,
  SHIFTF10 = 93,
  SHIFTF11 = 135,
  SHIFTF12 = 136,

  CTRLALTA = 30,
  CTRLALTB = 48,
  CTRLALTC = 46,
  CTRLALTD = 32,
  CTRLALTE = 18, // This one actually reports as 63 because on my locale it's the euro symbol.
  CTRLALTF = 33,
  CTRLALTG = 34,
  CTRLALTH = 35,
  CTRLALTI = 23,
  CTRLALTJ = 36,
  CTRLALTK = 37,
  CTRLALTL = 38,
  CTRLALTM = 50,
  CTRLALTN = 49,
  CTRLALTO = 24,
  CTRLALTP = 25,
  CTRLALTQ = 16,
  CTRLALTR = 19,
  CTRLALTS = 31,
  CTRLALTT = 20,
  CTRLALTU = 22,
  CTRLALTV = 47,
  CTRLALTW = 17,
  CTRLALTX = 45,
  CTRLALTY = 21,
  CTRLALTZ = 44,
};
#else
{
  BS = 127,
  TAB = 9,
  CTRLENTER = 10,
  ENTER = 13,
  CTRLBS = 8,

  ESC = 27,

  /*
   Used an algorithm in order to encode and fit up to 7 characters into a single int32_t
   Assuming characters are all lower than 0x80h (127d)

   The algorithm is works by shifting up to 4 characters 7 bits to the left, using 28 of 32 available bits.

   The last 3 bytes are guaranteed to be boolean data so each character fits in a single bit.
   The big endian bit is used to ensure that it is a control keybind starting with ESC.


   This way we can place 7 characters into 4 bytes, or an int32_t.

  */

  LEFT = 0xc0002200,
  UP = 0xc0002080,
  DOWN = 0xc0002100,
  RIGHT = 0xc0002180,

  INS = 0xc01f9900,
  DEL = 0xc01f9980,

  HOME = 0xc0002400,
  END = 0xc0002300,

  PGUP = 0xc01f9a80,
  PGDW = 0xc01f9b00,

  SHIFTTAB = 0xc0002d00,

  CTRLA = 1,
  CTRLB = 2,
  CTRLC = 3,
  CTRLD = 4,
  CTRLE = 5,
  CTRLF = 6,
  CTRLG = 7,
  CTRLH = 8,
  CTRLI = 9,
  CTRLJ = 10,
  CTRLK = 11,
  CTRLL = 12,
  CTRLM = 13,
  CTRLN = 14,
  CTRLO = 15,
  CTRLP = 16,
  CTRLQ = 17,
  CTRLR = 18,
  CTRLS = 19,
  CTRLT = 20,
  CTRLU = 21,
  CTRLV = 22,
  CTRLW = 23,
  CTRLX = 24,
  CTRLY = 25,
  CTRLZ = 26,

  /* Different naming is used between F1 and F4 (termios-related things) */
  F1 = 0xa0002800,
  F2 = 0xa0002880,
  F3 = 0xa0002900,
  F4 = 0xa0002980,

  F5 = 0xcfcd5880,
  F6 = 0xcfcdd880,
  F7 = 0xcfce1880,
  F8 = 0xcfce5880,
  F9 = 0xcfcc1900,
  F10 = 0xcfcc5900,
  F11 = 0xcfccd900,
  F12 = 0xcfcd1900,

  SHIFTF1 = 0xc64ed880,
  SHIFTF2 = 0xd64ed880,
  SHIFTF3 = 0xe64ed880,
  SHIFTF4 = 0xf64ed880,

  SHIFTF5 = 0xf76d5c70,
  SHIFTF6 = 0xf76ddc70,
  SHIFTF7 = 0xf76e1c70,
  SHIFTF8 = 0xf76e5c70,
  SHIFTF9 = 0xf76c1cf0,
  SHIFTF10 = 0xf76c5cf0,
  SHIFTF11 = 0xf76cdcf0,
  SHIFTF12 = 0xf76d1cf0,

  CTRLF1 = 0xc6aed880,
  CTRLF2 = 0xd6aed880,
  CTRLF3 = 0xe6aed880,
  CTRLF4 = 0xf6aed880,
  CTRLF5 = 0xd76d5c70,
  CTRLF6 = 0xd76ddc70,
  CTRLF7 = 0xd76e1c70,
  CTRLF8 = 0xd76e5c70,
  CTRLF9 = 0xd76c1cf0,
  CTRLF10 = 0xd76c5cf0,
  CTRLF11 = 0xd76cdcf0,
  CTRLF12 = 0xd76d1cf0,

  CTRLINS = 0xe6aed900,
  CTRLDEL = 0xe6aed980,
  CTRLHOME = 0xc6aed880,
  CTRLEND = 0xe6aed880,
  CTRLPGUP = 0xe6aeda80,
  CTRLPGDW = 0xe6aedb00,

  CTRLLEFT = 0xc6aed880,
  CTRLUP = 0xd6aed880,
  CTRLDOWN = 0xe6aed880,
  CTRLRIGHT = 0xf6aed880,

  SHIFTLEFT = 0xc64ed880,
  SHIFTUP = 0xd64ed880,
  SHIFTDOWN = 0xe64ed880,
  SHIFTRIGHT = 0xf64ed880,

  CTRLSHIFTLEFT = 0xc6ced880,
  CTRLSHIFTUP = 0xd6ced880,
  CTRLSHIFTDOWN = 0xe6ced880,
  CTRLSHIFTRIGHT = 0xf6ced880,

  ALTF1 = 0xc66ed880,
  ALTF2 = 0xd66ed880,
  ALTF3 = 0xe66ed880,
  ALTF4 = 0xf66ed880,
  ALTF5 = 0xf76d5c70,
  ALTF6 = 0xf76ddc70,
  ALTF7 = 0xf76e1c70,
  ALTF8 = 0xf76e5c70,
  ALTF9 = 0xf76c1cf0,
  ALTF10 = 0xf76c5cf0,
  ALTF11 = 0xf76cdcf0,
  ALTF12 = 0xf76d1cf0,

  CTRLALTF1 = 0xc6eed880,
  CTRLALTF2 = 0xd6eed880,
  CTRLALTF3 = 0xe6eed880,
  CTRLALTF4 = 0xf6eed880,
  CTRLALTF5 = 0xf76d5c70,
  CTRLALTF6 = 0xf76ddc70,
  CTRLALTF7 = 0xf76e1c70,
  CTRLALTF8 = 0xf76e5c70,
  CTRLALTF9 = 0xf76c1cf0,
  CTRLALTF10 = 0xf76c5cf0,
  CTRLALTF11 = 0xf76cdcf0,
  CTRLALTF12 = 0xf76d1cf0,

  CTRLSHIFTF1 = 0xc6ced880,
  CTRLSHIFTF2 = 0xd6ced880,
  CTRLSHIFTF3 = 0xe6ced880,
  CTRLSHIFTF4 = 0xf6ced880,
  CTRLSHIFTF5 = 0xf76d5c70,
  CTRLSHIFTF6 = 0xf76ddc70,
  CTRLSHIFTF7 = 0xf76e1c70,
  CTRLSHIFTF8 = 0xf76e5c70,
  CTRLSHIFTF9 = 0xf76c1cf0,
  CTRLSHIFTF10 = 0xf76c5cf0,
  CTRLSHIFTF11 = 0xf76cdcf0,
  CTRLSHIFTF12 = 0xf76d1cf0,

  ALTINS = 0xe66ed900,
  ALTHOME = 0xc66ed880,
  ALTPGUP = 0xe66eda80,
  ALTDEL = 0xe66ed980,
  ALTEND = 0xe66ed880,
  ALTPGDW = 0xe66edb00,

  ALTA = 0xa0184d80,
  ALTB = 0xa0188d80,
  ALTC = 0xa018cd80,
  ALTD = 0xa0190d80,
  ALTE = 0xa0194d80,
  ALTF = 0xa0198d80,
  ALTG = 0xa019cd80,
  ALTH = 0xa01a0d80,
  ALTI = 0xa01a4d80,
  ALTJ = 0xa01a8d80,
  ALTK = 0xa01acd80,
  ALTL = 0xa01b0d80,
  ALTM = 0xa01b4d80,
  ALTN = 0xa01b8d80,
  ALTO = 0xa01bcd80,
  ALTP = 0xa01c0d80,
  ALTQ = 0xa01c4d80,
  ALTR = 0xa01c8d80,
  ALTS = 0xa01ccd80,
  ALTT = 0xa01d0d80,
  ALTU = 0xa01d4d80,
  ALTV = 0xa01d8d80,
  ALTW = 0xa01dcd80,
  ALTX = 0xa01e0d80,
  ALTY = 0xa01e4d80,
  ALTZ = 0xa01e8d80,

  CTRLALTA = 0xa0004d80,
  CTRLALTB = 0xa0008d80,
  CTRLALTC = 0xa000cd80,
  CTRLALTD = 0xa0010d80,
  CTRLALTE = 0xa0014d80,
  CTRLALTF = 0xa0018d80,
  CTRLALTG = 0xa001cd80,
  CTRLALTH = 0xa0020d80,
  CTRLALTI = 0xa0024d80,
  CTRLALTJ = 0xa0028d80,
  CTRLALTK = 0xa002cd80,
  CTRLALTL = 0xa0030d80,
  CTRLALTM = 0xa0034d80,
  CTRLALTN = 0xa0038d80,
  CTRLALTO = 0xa003cd80,
  CTRLALTP = 0xa0040d80,
  CTRLALTQ = 0xa0044d80,
  CTRLALTR = 0xa0048d80,
  CTRLALTT = 0xa004cd80,
  CTRLALTS = 0xa0050d80,
  CTRLALTU = 0xa0054d80,
  CTRLALTV = 0xa0058d80,
  CTRLALTW = 0xa005cd80,
  CTRLALTX = 0xa0060d80,
  CTRLALTY = 0xa0064d80,
  CTRLALTZ = 0xa0068d80,
};
#endif

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <conio.h>
#include "win32/input_win32.c"
#else
#include "linux/input_linux.c"
#endif