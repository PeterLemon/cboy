/*************************************************************************
 *   Cboy, a Game Boy emulator
 *   Copyright (C) 2014 jkbenaim
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

#include "endian.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <sys/stat.h>
#include "types.h"
#include "cartdesc.h"
#include "libc.h"

struct header_s {
    /* 0x100 */ uint32_t entry;
    /* 0x104 */ uint8_t logo[48];
    /* 0x134 */ char title[16];
    //     /* 0x13f */ char manufacturer[4];
    //     /* 0x143 */ uint8_t cgb_flag;
    /* 0x144 */ char new_licensee_code[2];
    /* 0x146 */ uint8_t sgb_flag;
    /* 0x147 */ uint8_t cartridge_type;
    /* 0x148 */ uint8_t rom_size;
    /* 0x149 */ uint8_t ram_size;
    /* 0x14a */ uint8_t destination;
    /* 0x14b */ uint8_t old_licensee_code;
    /* 0x14c */ uint8_t rom_version;
    /* 0x14d */ uint8_t header_checksum;
    /* 0x14e */ uint16_t rom_checksum;
};

const uint8_t logo[48] =
{
    0xce, 0xed, 0x66, 0x66,
    0xcc, 0x0d, 0x00, 0x0b,
    0x03, 0x73, 0x00, 0x83,
    0x00, 0x0c, 0x00, 0x0d,
    0x00, 0x08, 0x11, 0x1f,
    0x88, 0x89, 0x00, 0x0e,
    0xdc, 0xcc, 0x6e, 0xe6,
    0xdd, 0xdd, 0xd9, 0x99,
    0xbb, 0xbb, 0x67, 0x63,
    0x6e, 0x0e, 0xec, 0xcc,
    0xdd, 0xdc, 0x99, 0x9f,
    0xbb, 0xb9, 0x33, 0x3e
};

static const unsigned int max_rom_size = 8 * 1024 * 1024;

int cmd_info_impl( int argc, char *argv[] )
{
    uint32_t rom_size = 0;
    extern const uint8_t __cartrom[];
    extern const unsigned __cartrom_size;
    const uint8_t *rom = __cartrom;
    int i = 0;
    uint16_t my_checksum = 0;
    uint8_t my_header_checksum = 0;

    if( argc < 3 )
    {
        return 1;
    }

    if( __cartrom_size > max_rom_size )
        rom_size = max_rom_size;
    else
        rom_size = __cartrom_size;

    struct header_s header;
    memcpy( &header, rom+0x100, 0x50 );

    //   /* 0x100 */ uint8_t entry[4];

    //     /* 0x104 */ uint8_t logo[48];

    //     /* 0x134 */ char title[16];
    //     /* 0x13f */ char manufacturer[4];
    //     /* 0x143 */ uint8_t cgb_flag;
    // TODO: clean up title printing

    //     /* 0x144 */ char new_licensee_code[2];

    //     /* 0x146 */ uint8_t sgb_flag;

    //     /* 0x147 */ uint8_t cartridge_type;

    //     /* 0x148 */ uint8_t rom_size;

    //     /* 0x149 */ uint8_t ram_size;

    //     /* 0x14a */ uint8_t destination;

    //     /* 0x14b */ uint8_t old_licensee_code;

    //     /* 0x14c */ uint8_t rom_version;

    //     /* 0x14d */ uint8_t header_checksum;
    my_header_checksum = 0;
    for(i=0x134;i<=0x14c;++i)
        my_header_checksum = my_header_checksum - rom[i]-1;


    //     /* 0x14e */ uint16_t rom_checksum;
    my_checksum = 0;
    for(i=0;i<rom_size; i++)
        my_checksum += rom[i];
    my_checksum -= rom[0x14e];
    my_checksum -= rom[0x14f];

    return 0;
}
