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

#include "cart.h"
#include "memory.h"
#include "mbc.h"
#include "cartdesc.h"
#include "cpu.h"
#include "bootrom.h"

struct cart_s cart;

char* romname2savename( char* savename, char* romname, int savenamelength )
{
  if( romname == NULL )
  {
    // uhhhh
    return NULL;
  }
  
  const char* suffix = strrchr( romname, (int)'.' );
  
  if( suffix == NULL )
  {
    // romname doesn't have a dot
    
    // copy the romname to savename
    strncpy( savename, romname, savenamelength - 4 );
    
    // append ".sav" to the savename
    strcat( savename, ".sav" );
  }
  else
  {
    // romname has a dot and suffix
    
    int prefixlength = strlen( romname ) - strlen( suffix );
    
    // copy the romname (up to the dot) to savename
    strncpy( savename, romname, prefixlength );
    
    // null-terminate it
    savename[prefixlength] = '\0';
    
    // append ".sav" to the savename
    strcat( savename, ".sav" );
  }
  
  return savename;
}

void cart_init( void ) {
  char *bootromName = NULL;
  char *cartromName = "n64chain";

  // cart_init_cartrom also sets cart.savename
  cart_init_cartrom( cartromName );
  cart_init_bootrom( bootromName );
  
  // set the MBC type for later use
  cart.mbc_type = cart.cartrom[0x147];
  
  // determine the emulated extram size
  switch( cart.cartrom[0x149] )
  {
    case 0:
      cart.extram_size = 0;
      break;
    case 1:
      cart.extram_size = 2048;
      break;
    case 2:
      cart.extram_size = 8192;
      break;
    case 3:
      cart.extram_size = 32768;
      break;
    case 4:
      cart.extram_size = 131072;
      break;
  }
  // exception: MBC2 and MBC7 always have extram
  switch( cart.mbc_type )
  {
    case 0x05:  // MBC2
    case 0x06:  // MBC2+BATTERY
      cart.extram_size = 512;
      break;
    case 0x22: // MBC7+?
      cart.extram_size = 32768;
      break;
  }
  uint8_t extram[131072];
  uint8_t extramValidRead[131072];
  uint8_t extramValidWrite[131072];
  // allocate memory for extram
  cart.extram = extram;
  cart.extramValidRead = extramValidRead;
  cart.extramValidWrite = extramValidWrite;
  
  // determine whether the extram (if any) is battery-backed
  switch( cart.mbc_type )
  {
    case 0x03:  // MBC1+RAM+BATTERY
    case 0x06:  // MBC2+BATTERY
    case 0x0F:  // MBC3+TIMER+BATTERY
    case 0x10:  // MBC3+TIMER+RAM+BATTERY
    case 0x13:  // MBC3+RAM+BATTERY
    case 0x1B:  // MBC5+RAM+BATTERY
    case 0x1E:  // MBC5+RUMBLE+RAM+BATTERY
    case 0x22:  // MBC7+RAM+BATTERY
    case 0xFC:  // POCKET CAMERA
    case 0xFE:  // HuC3
    case 0xFF:  // HuC1
      cart.battery_backed = 1;
      break;
    default:
      cart.battery_backed = 0;
      break;
  }
  
  // load extram from file if battery backed
  if( cart.battery_backed && (cart.extram_size > 0) )
  {
  }
  
  cart_reset_mbc();
  
}

void cart_init_cartrom( char* cartromName )
{
  extern uint8_t __cartrom[];
  extern const unsigned __cartrom_size;

  if( __cartrom_size > MAX_CARTROM_SIZE )
    cart.cartromsize = MAX_CARTROM_SIZE;
  else
    cart.cartromsize = __cartrom_size;
 
  // Read the cartrom.
  cart.cartrom = __cartrom;
  //printf( "Cart rom: %d bytes read.\n", cart.cartromsize );
  
  cart.cartrom_num_banks = cart.cartromsize / 16384;
  if( cart.cartromsize % 16384 != 0 )
  {
    cart.cartrom_num_banks++;
  }
  
  // Set savename
  romname2savename( cart.savename, cartromName, 256 );
}

void cart_init_bootrom( char* bootromName )
{
  cart.bootromsize = bootrom_bin_len;
  cart.bootrom = (uint8_t *)bootrom_bin;
}

/*
 * void cart_reset_mbc( void )
 * This function installs the appropriate handlers
 * for the current MBC.
 */
void cart_reset_mbc()
{
  if( state.bootRomEnabled )
  {
    mbc_boot_install();
    return;
  }
  
  // install MBC driver
//   mbc_sim_install();
  switch( cart.mbc_type )
  {
    case 0x00:  // ROM ONLY
//     case 0x08:  // ROM+RAM           // no known games use these
//     case 0x09:  // ROM+RAM+BATTERY
      mbc_none_install();
      break;
    case 0x01:  // MBC1
    case 0x02:  // MBC1+RAM
    case 0x03:  // MBC1+RAM+BATTERY
      mbc_mbc1_install();
      break;
    case 0x05:  // MBC2
    case 0x06:  // MBC2+BATTERY
      mbc_mbc2_install();
      break;
    case 0x0F:	// MBC3+TIMER+BATTERY
    case 0x10:	// MBC3+TIMER+RAM+BATTERY
    case 0x11:	// MBC3
    case 0x12:	// MBC3+RAM
    case 0x13:	// MBC3+RAM+BATTERY
      mbc_mbc3_install();
      break;
    case 0x19:  // MBC5
    case 0x1A:  // MBC5+RAM
    case 0x1B:  // MBC5+RAM+BATTERY
    case 0x1C:  // MBC5+RUMBLE
    case 0x1D:  // MBC5+RUMBLE+RAM
    case 0x1E:  // MBC5+RUMBLE+RAM+BATTERY
      mbc_mbc5_install();
      break;
    case 0x22:  // MBC7+?
      mbc_mbc7_install();
      break;
    case 0xFC:	// POCKET CAMERA
      mbc_cam_install();
      break;
    case 0xFE:  // HuC3
      mbc_huc3_install();
      break;
    case 0xFF:  // HuC1
      // TODO
      mbc_huc1_install();
      break;
    default:
      // danger danger
      exit(1);
      break;
  }
}
  
