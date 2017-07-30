/*************************************************************************
 *   Cboy, a Game Boy emulator
 *   Copyright (C) 2012 jkbenaim
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ************************************************************************/

#ifndef _CPU_H_
#define _CPU_H_

#include "types.h"
#include "endian.h"

#ifdef __cplusplus
extern "C" {
#endif

struct state_s {
  pc_t pc;
  uint16_t sp;
#ifdef __BIG_ENDIAN__
  union {
    struct {
      uint8_t h;
      uint8_t l;
    };
    uint16_t hl;
  };
  union {
    struct {
      uint8_t d;
      uint8_t e;
    };
    uint16_t de;
  };
  union {
    struct {
      uint8_t b;
      uint8_t c;
    };
    uint16_t bc;
  };
  union {
    struct {
      uint8_t a;
      uint8_t f;
    };
    uint16_t af;
  };
#else	// __LITTLE_ENDIAN__
  union {
    struct {
      uint8_t l;
      uint8_t h;
    };
    uint16_t hl;
  };
  union {
    struct {
      uint8_t e;
      uint8_t d;
    };
    uint16_t de;
  };
  union {
    struct {
      uint8_t c;
      uint8_t b;
    };
    uint16_t bc;
  };
  union {
    struct {
      uint8_t f;
      uint8_t a;
    };
    uint16_t af;
  };
#endif
  char flag_c;
  char flag_h;
  char flag_n;
  char flag_z;
  int vid_mode;
  int old_vid_mode;
  uint8_t joyp;
  uint8_t joyp_buttons;
  uint8_t joyp_directions;
  uint8_t joyp_select;
  uint32_t masterClock; // incremented at 1,048,576 Hz
  uint32_t lastMasterClock;
  uint8_t div, tima, tma, tac;	// timers
  int serialBitsSent;	// bits sent over link
  int serialClocksUntilNextSend;
  int serialTimeoutClock;
  uint8_t sb, sc;
  uint8_t lcdc;
  uint8_t stat;
  uint8_t pending_stat_interrupts;
  uint8_t scy;
  uint8_t scx;
  uint8_t ly;
  int line_progress;
  uint8_t lyc;
  uint8_t last_ly;
  uint8_t bgp;
  uint8_t obp0, obp1;
  uint8_t wx,wy;
  uint8_t last_line_rendered;
  int bootRomEnabled;	// 0 = disabled, 1 = enabled
  int ime;		// set to IME_ENABLED or IME_DISABLED
  int ie, iflag;
  int halt;		// 0 = not halted, 1 = halted
  uint8_t op;
  uint8_t cb_op;
  int halt_glitch;
  int frame_done;
  int instr_time;
  uint8_t key1;
  int cpu_speed;
  
  // sound stuff
  uint8_t nr10, nr11, nr12, nr13, nr14, nr20, nr21, nr22, nr23, nr24, nr30, nr31;
  uint8_t nr32, nr33, nr34, nr41, nr42, nr43, nr44, nr50, nr51, nr52;
  uint8_t waveram[0x10];
  
  //cgb stuff
  uint8_t bgpi;
  uint8_t bgpd[0x40];
  uint8_t obpi;
  uint8_t obpd[0x40];
  uint8_t vbk;
  uint8_t caps;
  uint8_t svbk;
#ifdef __BIG_ENDIAN__
  union {
    struct {
      uint8_t hdma1;
      uint8_t hdma2;
    };
    uint16_t hdma_source;
  };
  union {
    struct {
      uint8_t hdma3;
      uint8_t hdma4;
    };
    uint16_t hdma_destination;
  };
#else	// __LITTLE_ENDIAN__
  union {
    struct {
      uint8_t hdma2;
      uint8_t hdma1;
    };
    uint16_t hdma_source;
  };
  union {
    struct {
      uint8_t hdma4;
      uint8_t hdma3;
    };
    uint16_t hdma_destination;
  };
#endif
  uint8_t hdma5;
}; // state

extern struct state_s state;


/*
 * Flags register masks
 */
#define FLAGS_NOTUSED_0		0x01
#define FLAGS_NOTUSED_1		0x02
#define FLAGS_NOTUSED_2		0x04
#define FLAGS_NOTUSED_3		0x08
#define FLAGS_C			0x10
#define FLAGS_H			0x20
#define FLAGS_N			0x40
#define FLAGS_Z			0x80

/*
 * Flag (re)set macros
 */
// #define SET_C()			(state.flag_c = 1)
// #define SET_H()			(state.flag_h = 1)
// #define SET_N()			(state.flag_n = 1)
// #define SET_Z()			(state.flag_z = 1)
// #define RESET_C()		(state.flag_c = 0)
// #define RESET_H()		(state.flag_h = 0)
// #define RESET_N()		(state.flag_n = 0)
// #define RESET_Z()		(state.flag_z = 0)
// #define ISSET_C()		(state.flag_c != 0)
// #define ISSET_H()               (state.flag_h != 0)
// #define ISSET_N()               (state.flag_n != 0)
// #define ISSET_Z()               (state.flag_z != 0)
#define SET_C()                 (state.f |= FLAGS_C)
#define SET_H()                 (state.f |= FLAGS_H)
#define SET_N()                 (state.f |= FLAGS_N)
#define SET_Z()                 (state.f |= FLAGS_Z)
#define RESET_C()               (state.f &= ~FLAGS_C)
#define RESET_H()               (state.f &= ~FLAGS_H)
#define RESET_N()               (state.f &= ~FLAGS_N)
#define RESET_Z()               (state.f &= ~FLAGS_Z)
#define ISSET_C()               (state.f & FLAGS_C)
#define ISSET_H()               (state.f & FLAGS_H)
#define ISSET_N()               (state.f & FLAGS_N)
#define ISSET_Z()               (state.f & FLAGS_Z)

/*
 * Number of cycles for each video mode.
 * {  mode 2: 80 cycles
 *    mode 3: 172 cycles
 *    mode 0: 204 cycles  } x 144
 * mode 1: 4560 cycles
 */
#define CYCLES_MODE_2	80
#define CYCLES_MODE_3	172
#define CYCLES_MODE_0	204
#define CYCLES_MODE_1	4560
#define CYCLES_LINE	(CYCLES_MODE_2 + CYCLES_MODE_3 + CYCLES_MODE_0)
#define CYCLES_FRAME	70224
#define CYCLES_RENDER_LINE	(CYCLES_MODE_2 + CYCLES_MODE_3)

/*
 * Interrupt masks
 */
#define IMASK_VBLANK	0x01
#define IMASK_LCD_STAT	0x02
#define IMASK_TIMER	0x04
#define IMASK_SERIAL	0x08
#define IMASK_JOYPAD	0x10
#define IME_DISABLED	0x00
#define IME_ENABLED	0xFF

/*
 * Interrupt queue message names
 */
typedef enum {
  INTERRUPT_VBLANK = 1,
  INTERRUPT_LCD_STAT,
  INTERRUPT_TIMER,
  INTERRUPT_SERIAL,
  INTERRUPT_JOYPAD
} InterruptMessage_t;

int cpu_init( void );
void cpu_do_one_instruction( void );

__attribute__((hot))
void cpu_do_one_frame( void );
uint8_t* cpu_getReg( int );
uint8_t cpu_get_flags_register( void );
void cpu_set_flags_register( uint8_t );

extern int stop;

#ifdef __cplusplus
}
#endif


#endif // !_CPU_H_
