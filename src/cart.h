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

#ifndef _CART_H_
#define _CART_H_
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct cart_s {
  int mbc_type;
  int cartrom_num_banks;
  int reg_rom_bank_low;	// low bits of ROM bank
  int reg_rom_bank_high;  // high bits of ROM bank
  uint8_t* bootrom;
  size_t bootromsize;
  uint8_t* cartrom;
  uint8_t* cartromValid;
  uint8_t* cartrom_bank_zero;
  uint8_t* cartrom_bank_n;
  uint8_t* cartromValid_bank_n;
  uint16_t cart_bank_num;
  size_t cartromsize;
  uint8_t* extram;
  uint8_t* extramValidRead;
  uint8_t* extramValidWrite;
  uint8_t* extram_bank;
  uint8_t* extram_bank_validRead;
  uint8_t* extram_bank_validWrite;
  uint8_t extram_bank_num;
  int extramEnabled;
  size_t extram_size;
  int extram_num_banks;
  int battery_backed;
  char savename[256];
  int huc3_ram_mode;
};

#define MAX_CARTROM_SIZE	8388608
#define MAX_BOOTROM_SIZE	2304

__attribute__((cold))
extern void cart_init( void );
extern void cart_disable_bootrom( void );
__attribute__((cold))
void cart_init_cartrom( char* cartromName );
__attribute__((cold))
extern void cart_init_bootrom( void );
__attribute__((cold))
extern void cart_reset_mbc( void );
extern struct cart_s cart;

#ifdef __cplusplus
}
#endif

#endif // _CART_H_
