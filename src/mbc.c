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

#include "mbc.h"
#include "types.h"


/*
 * int mbc_romSize2numBanks( uint8_t romSize )
 * 
 * Given the romSize byte from the cart header, returns
 * the size of the rom.
 */
__attribute__((cold))
int mbc_romSize2numBanks( uint8_t romSize )
{
  static const unsigned lut[] __attribute__((aligned(16))) = {
    2, 4, 8, 16, 32, 64, 128, 256
  };

  switch( romSize )
  {
    case 0x52:
      return 72;
      break;
    case 0x53:
      return 80;
      break;
    case 0x54:
      return 96;
      break;
    default:
      if (romSize < 8)
        return lut[romSize];
      // uh-oh
      return 2;
      break;
  }
}

__attribute__((cold))
int mbc_ramSize2numBytes( uint8_t ramSize )
{
  static const unsigned lut[] __attribute__((aligned(16))) = {
    0, 2048, 8192, 32768
  };

  if (ramSize >= 4)
    return 0;

  return lut[ramSize];
}

