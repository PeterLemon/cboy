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

#include "types.h"
#include "video.h"
#include "memory.h"
#include "cpu.h"
#include <priv_include/os/fbtext.h>
#include <rcp/vi.h>

static pixel_t pixmem[160*144] __attribute__((aligned(16)));
static pixel_t colormem[160*144] __attribute__((aligned(16)));

static pixel_t myCachedPalettes[8][8] __attribute__((aligned(16)));
static pixel_t pixelscolors[2][8] __attribute__((aligned(32)));
static pixel_t myPalette[8] __attribute__((aligned(16)));
char inval_palette = 1;

__attribute__((hot))
static int vid_drawSpanCommon(pixel_t *palette, int vramAddr, int x, int y, int vramBank,
        int xFlip, int *lineStart, int *spanStart, int *spanEnd) {

    // Is this span off the left of the screen?
    if(x<-7)
        return 1;

    // Is this span off the top of the screen?
    if(y<0)
        return 1;

    if((x-8)>160)
        return 1;

    *lineStart = 160 * y;
    //   uint32_t *pixmem = (uint32_t*) screen->pixels;

    // fill pixel array with span
    int lowBits, highBits;
    if( vramBank == 0 )
    {
        lowBits = vram_bank_zero[vramAddr];
        highBits = vram_bank_zero[vramAddr + 1];
    }
    else
    {
        lowBits = vram_bank_one[vramAddr];
        highBits = vram_bank_one[vramAddr + 1];
    }

    // pixel 7 at the left, pixel 0 at the right
    __builtin_mips_cache(0xD, pixelscolors[0]);
    __builtin_mips_cache(0xD, pixelscolors[1]);

    int p;
    for( p=0; p<8; ++p )
    {
        int color;
        int mask;
        color = 0;
        mask = 1<<p;
        if( lowBits & mask )
            color = 1;
        if( highBits & mask )
            color += 2;
        pixelscolors[0][p] = palette[color];
        pixelscolors[1][p] = color;
    }

    // Is the span partially offscreen?
    *spanStart = 0;
    *spanEnd = 7;
    // Partially off the left side of the screen
    if( x < 0 )
        *spanStart = -x;
    // Partially off the right side of the screen
    if( x > (160-8) )
        *spanEnd = 159 - x;

    // xFlip?
    if( xFlip )
    {
        uint32_t scratch1, scratch2;

        __asm__ __volatile__(
          ".set noat\n\t"
          ".set noreorder\n\t"
          "1:\n\t"
          "lw %0, 0x0(%2)\n\t"
          "lw %1, 0xC(%2)\n\t"
          "sh %0, 0xC(%2)\n\t"
          "srl %0, %0, 0x8\n\t"
          "sh %0, 0xE(%2)\n\t"
          "sh %1, 0x0(%2)\n\t"
          "srl %1, %1, 0x8\n\t"
          "sh %1, 0x2(%2)\n\t"
          "addiu %2, %2, 0x10\n\t"

          "lw %0, (0x4-0x10)(%2)\n\t"
          "lw %1, (0x8-0x10)(%2)\n\t"
          "sh %0, (0x8-0x10)(%2)\n\t"
          "srl %0, %0, 0x8\n\t"
          "sh %0, (0xA-0x10)(%2)\n\t"
          "andi %0, %2, 0x20\n\t"
          "sh %1, (0x4-0x10)(%2)\n\t"
          "srl %1, %1, 0x8\n\t"
          "sh %1, (0x6-0x10)(%2)\n\t"
          "beq %0, $0, 1b\n\t"
          "nop\n\t"
          "addiu %2, %2, -0x20\n\t"

          : "=&r"(scratch1), "=&r"(scratch2) //, "=&r"(pixelscolors[0])
          : "r"(pixelscolors[0])
          : "memory"
        );
    }

    return 0;
}


__attribute__((hot))
static void vid_drawOpaqueSpan( uint8_t pal, uint16_t vramAddr, int x, int y, int vramBank, int xFlip, int updateColormem ) {
    int lineStart, spanStart, spanEnd;
    if (vid_drawSpanCommon(&myCachedPalettes[pal][0], vramAddr, x, y, vramBank, xFlip, &lineStart, &spanStart, &spanEnd))
      return;

    // Draw the span from left to right.
    unsigned didx = lineStart + x + spanStart;
    unsigned sidx = 7 - spanStart;

    for( int p=spanStart; p<=spanEnd; ++p )
    {
        if( updateColormem ) {
          colormem[ didx ] = pixelscolors[1][ sidx ];
        }

        pixmem[ didx++ ] = pixelscolors[0][ sidx-- ];
    }

    __builtin_mips_cache(0x11, pixelscolors[0]);
    __builtin_mips_cache(0x11, pixelscolors[1]);
}

__attribute__((hot))
static void vid_drawTransparentSpan( uint8_t pal, uint16_t vramAddr, int x, int y, int vramBank, int xFlip, int priority ) {
    int lineStart, spanStart, spanEnd;
    if (vid_drawSpanCommon(&myCachedPalettes[pal][4], vramAddr, x, y, vramBank, xFlip, &lineStart, &spanStart, &spanEnd))
      return;

    // Draw the span from left to right.
    int p;
    for( p=spanStart; p<=spanEnd; ++p )
    {
        if( pixelscolors[1][7-p] != 0 )
        {
            if( priority )
            {
                if( colormem[ lineStart + x + p ] == 0 )
                {
                    pixmem[ lineStart + x + p ] = pixelscolors[0][ 7-p ];
                } else {
                    // Uncomment the next line to highlight the
                    // sprite-behind-background case in bright red.
                    //           pixmem[ lineStart + x + p ] = 0x001f; // red
                }
            } else {
                pixmem[ lineStart + x + p ] = pixelscolors[0][ 7-p ];
            }
        }
    }

    __builtin_mips_cache(0x11, pixelscolors[0]);
    __builtin_mips_cache(0x11, pixelscolors[1]);
}

__attribute__((hot))
void vid_render_line()
{
    // If the LCD is off, then return.
    // We should probably blank the line instead...
    if( (state.lcdc & LCDC_LCD_ENABLE) == 0 )
        return;

    // Set up cached palettes.
    if ( inval_palette ) {
    if( state.caps & 0x04 )
    {
        __builtin_mips_cache(0xD, myPalette);

        // DMG mode
        // colors need to be translated through BOTH the DMG and CGB palettes
        for (int i = 0; i < 8; i++) {
          __builtin_mips_cache(0xD, myCachedPalettes[i]);

          myPalette[4] = state.bgpd[(i*8)+0] + (state.bgpd[(i*8)+1]<<8);
          myPalette[5] = state.bgpd[(i*8)+2] + (state.bgpd[(i*8)+3]<<8);
          myPalette[6] = state.bgpd[(i*8)+4] + (state.bgpd[(i*8)+5]<<8);
          myPalette[7] = state.bgpd[(i*8)+6] + (state.bgpd[(i*8)+7]<<8);

          myCachedPalettes[i][0] = myPalette[4 + ((state.bgp)      & 0x3) ];
          myCachedPalettes[i][1] = myPalette[4 + ((state.bgp >> 2) & 0x3) ];
          myCachedPalettes[i][2] = myPalette[4 + ((state.bgp >> 4) & 0x3) ];
          myCachedPalettes[i][3] = myPalette[4 + ((state.bgp >> 6) & 0x3) ];

          myPalette[4] = state.obpd[(i*8)+0] + (state.obpd[(i*8)+1]<<8);
          myPalette[5] = state.obpd[(i*8)+2] + (state.obpd[(i*8)+3]<<8);
          myPalette[6] = state.obpd[(i*8)+4] + (state.obpd[(i*8)+5]<<8);
          myPalette[7] = state.obpd[(i*8)+6] + (state.obpd[(i*8)+7]<<8);

          int dmgPalette = (i==0) ? state.obp0 : state.obp1;

          myCachedPalettes[i][4] = myPalette[4 + ((dmgPalette)      & 0x3) ];
          myCachedPalettes[i][5] = myPalette[4 + ((dmgPalette >> 2) & 0x3) ];
          myCachedPalettes[i][6] = myPalette[4 + ((dmgPalette >> 4) & 0x3) ];
          myCachedPalettes[i][7] = myPalette[4 + ((dmgPalette >> 6) & 0x3) ];
        }
    } else {
        // CGB mode
        for (int i = 0; i < 8; i++) {
          __builtin_mips_cache(0xD, myCachedPalettes[i]);

          myCachedPalettes[i][0] = state.bgpd[(i*8)+0] + (state.bgpd[(i*8)+1]<<8);
          myCachedPalettes[i][1] = state.bgpd[(i*8)+2] + (state.bgpd[(i*8)+3]<<8);
          myCachedPalettes[i][2] = state.bgpd[(i*8)+4] + (state.bgpd[(i*8)+5]<<8);
          myCachedPalettes[i][3] = state.bgpd[(i*8)+6] + (state.bgpd[(i*8)+7]<<8);
          myCachedPalettes[i][4] = state.obpd[(i*8)+0] + (state.obpd[(i*8)+1]<<8);
          myCachedPalettes[i][5] = state.obpd[(i*8)+2] + (state.obpd[(i*8)+3]<<8);
          myCachedPalettes[i][6] = state.obpd[(i*8)+4] + (state.obpd[(i*8)+5]<<8);
          myCachedPalettes[i][7] = state.obpd[(i*8)+6] + (state.obpd[(i*8)+7]<<8);
        }
    }
    inval_palette = 0;
    }

    int backLineToRender = ((int)state.ly + (int)state.scy) % 256;
    int backTileRow = backLineToRender / 8;
    int backLineInTile = backLineToRender % 8;

    int i, tileNum, tileAddress, tileDataAddress, tile, spanAddress, vramBank;

    // Render the background.
    if( state.lcdc & LCDC_BG_ENABLE )
        for( i=-1; i<21; ++i )
        {
            tileNum = backTileRow*32 + (i + (state.scx >> 3))%32;


            if( state.lcdc & LCDC_BG_TILE_MAP_SELECT )
                tileAddress = 0x1C00 + tileNum;
            else
                tileAddress = 0x1800 + tileNum;


            if( state.lcdc & LCDC_TILE_DATA_SELECT )
            {
                tile = vram_bank_zero[tileAddress];
                tileDataAddress = 0x0000 + tile*16;
            }
            else
            {
                tile = vram_bank_zero[tileAddress];
                if(tile < 0x80)
                    tileDataAddress = 0x1000 + tile*16;
                else
                    tileDataAddress = tile*16;
            }


            uint8_t attributes = vram_bank_one[tileAddress];
            int xFlip = (attributes&BG_XFLIP)?1:0;
            int yFlip = (attributes&BG_YFLIP)?1:0;

            if( yFlip )
                spanAddress = tileDataAddress + ((7-backLineInTile) * 2);
            else
                spanAddress = tileDataAddress + (backLineInTile * 2);

            // Set the VRAM bank.
            if( state.caps == 0x04 )
                vramBank = 0;
            else
                vramBank = (attributes & BG_VRAM_BANK)?1:0;

            // Set the palette.
            uint8_t pal;
            if( state.caps == 0x04 )
                pal = 0;    // dmg mode
            else
                pal = attributes & 0x07;    // cgb mode

            vid_drawOpaqueSpan( pal, spanAddress, i*8 - (state.scx%8), state.ly, vramBank, xFlip, 1 );
        }

    // Render the window.
    int windowLineToRender = (int)state.ly - (int)state.wy;
    int windowTileRow = windowLineToRender / 8;
    int windowLineInTile = windowLineToRender % 8;
    if( (state.lcdc & LCDC_WINDOW_ENABLE) && (state.ly >= state.wy) )
    {
        for( i=0; i<20; ++i )
        {
            tileNum = windowTileRow*32 + i;

            if( state.lcdc & LCDC_WINDOW_TILE_MAP_SELECT )
                tileAddress = 0x1C00 + tileNum;
            else
                tileAddress = 0x1800 + tileNum;


            if( state.lcdc & LCDC_TILE_DATA_SELECT )
            {
                tile = vram_bank_zero[tileAddress];
                tileDataAddress = 0x0000 + tile*16;
            }
            else
            {
                tile = vram_bank_zero[tileAddress];
                if(tile < 0x80)
                    tileDataAddress = 0x1000 + tile*16;
                else
                    tileDataAddress = tile*16;
            }

            uint8_t attributes = vram_bank_one[tileAddress];
            int xFlip = (attributes&BG_XFLIP)?1:0;
            int yFlip = (attributes&BG_YFLIP)?1:0;

            if( yFlip )
                spanAddress = tileDataAddress + ((7-windowLineInTile) * 2);
            else
                spanAddress = tileDataAddress + (windowLineInTile * 2);

            // Set the VRAM bank.
            if( state.caps == 0x04 )
                vramBank = 0;
            else
                vramBank = (attributes & BG_VRAM_BANK)?1:0;

            // Set the palette.
            uint8_t pal;
            if( state.caps == 0x04 )
                pal = 0;        // dmg mode
            else
                pal = attributes & 0x07;        // cgb mode

            vid_drawOpaqueSpan( pal, spanAddress, i*8 + state.wx - 7, state.ly, vramBank, xFlip, 1 );
        }
    }

    int objLineToRender = ((int)state.ly) % 256;

    // Render sprites.
    if( state.lcdc & LCDC_OBJ_DISPLAY )
        for( i=0; i<40; ++i )
        {
            uint8_t y          = oam[(i<<2)    ];
            uint8_t x          = oam[(i<<2) + 1];
            uint8_t tileNum    = oam[(i<<2) + 2];
            uint8_t attributes = oam[(i<<2) + 3];

            // Set object height.
            int objHeight;
            if( state.lcdc & LCDC_OBJ_SIZE )
                objHeight = 16;
            else
                objHeight = 8;

            if(objHeight == 16)
            {
                tileNum &= 0xfe;
            }

            // Is this sprite on this line? If not, let's skip it.
            int reject = 0;
            int lineOfSpriteToRender = objLineToRender - y + 16;
            // yFlip is handled here
            if( attributes & SPRITE_YFLIP )
                lineOfSpriteToRender = objHeight - lineOfSpriteToRender - 1;
            if( lineOfSpriteToRender >= objHeight || lineOfSpriteToRender < 0  )
                reject = 1;
            if( reject == 1 )
                continue;

            // Set the palette.
            uint8_t pal;
            if( state.caps == 0x04 )
                if( attributes & SPRITE_DMG_PAL )   // dmg mode
                    pal = 1;
                else
                    pal = 0;
            else
                pal = attributes & 0x07;    // cgb mode

            // Set the VRAM bank.
            if( state.caps == 0x04 )
                vramBank = 0;   // dmg mode
            else
                vramBank = (attributes & SPRITE_VRAM_BANK)?1:0; // cgb mode

            // Set the priority. 0=sprite above background, 1=sprite behind bg color 1-3
            int priority = attributes & SPRITE_PRIORITY;

            // Calculate the span address, relative to 0x8000.
            spanAddress = (tileNum * 16) + (lineOfSpriteToRender*2);

            int xFlip = attributes & SPRITE_XFLIP;
            vid_drawTransparentSpan( pal, spanAddress, x-8, state.ly, vramBank, xFlip, priority );
        }


}

// These pre-defined values are suitable for NTSC.
// TODO: Add support for PAL and PAL-M televisions.
static const vi_state_t vi_state = {
  0x0000324E, // status
  0x00200000, // origin
  0x00000140, // width
  0x00000002, // intr
  0x00000000, // current
  0x03E52239, // burst
  0x0000020D, // v_sync
  0x00000C15, // h_sync
  0x0C150C15, // leap
  0x006C02EC, // h_start
  0x002501FF, // v_start
  0x000E0204, // v_burst
  0x00000200, // x_scale
  0x00000400, // y_scale
};

struct libn64_fbtext_context fbtext;

void vid_init()
{
  vi_flush_state(&vi_state);

  for (unsigned i = 0; i < 320 * 240 * 2; i += 16) {
    __asm__ __volatile__(
      ".set gp=64\n\t"
      "cache 0xD, 0x0(%0)\n\t"
      "sd $zero, 0x0(%0)\n\t"
      "sd $zero, 0x8(%0)\n\t"
      "cache 0x19, 0x0(%0)\n\t"
      ".set gp=default\n\t"

      :: "r" (0x80000000 | (vi_state.origin + i))
      : "memory"
    );
  }

  libn64_fbtext_init(&fbtext, vi_state.origin, LIBN64_FBTEXT_COLOR_WHITE,
      LIBN64_FBTEXT_COLOR_BLACK, 0x140, LIBN64_FBTEXT_16BPP);

  fbtext.x = 13; fbtext.y = 1;
  libn64_fbtext_puts(&fbtext, "cboy by jrra\n");
  fbtext.x = 3; fbtext.y = 13;
  libn64_fbtext_puts(&fbtext, "n64chain port by marathonm & krom\n");
}

void vid_waitForNextFrame()
{
}

void vid_frame()
{
  uint32_t fbaddr;
  unsigned i, j;

  fbaddr = 0x80000000 | (vi_state.origin + 48 * 320 *2 + 80 * 2);

  for (i = 0; i < 144; i++) {
    for (j = 0; j < 160; j += 16) {
      uint32_t clh1, clh2;

      __asm__ __volatile__(
        ".set noat\n\t"
        ".set gp=64\n\t"

        // Color Bits Are In ABBBBBGGGGGRRRRR Order, N64 Needs RRRRRGGGGGBBBBBA Order
        "li %5, 0x001F001F\n\t" // R Bits: 0x001F001F001F001F
        "dsll32 %5, %5, 0\n\t"
        "li %0, 0x001F001F\n\t"
        "or %5, %5, %0\n\t"
        "li %6, 0x03E003E0\n\t" // G Bits: 0x03E003E003E003E0
        "dsll32 %6, %6, 0\n\t"
        "li %0, 0x03E003E0\n\t"
        "or %6, %6, %0\n\t"
        "li %7, 0x7C007C00\n\t" // B Bits: 0x7C007C007C007C00
        "dsll32 %7, %7, 0\n\t"
        "li %0, 0x7C007C00\n\t"
        "or %7, %7, %0\n\t"

        "ld %0, 0x0(%4)\n\t"
        "and %1, %0, %5\n\t"
        "dsll %1, %1, 11\n\t"
        "and %8, %0, %6\n\t"
        "dsll %8, %8, 1\n\t"
        "or %1, %1, %8\n\t"
        "and %8, %0, %7\n\t"
        "dsrl %8, %8, 9\n\t"
        "or %1, %1, %8\n\t"
        "sd %1, 0x0(%3)\n\t"

        "addiu %2, %3, 0x10\n\t"

        "ld %0, 0x8(%4)\n\t"
        "and %1, %0, %5\n\t"
        "dsll %1, %1, 11\n\t"
        "and %8, %0, %6\n\t"
        "dsll %8, %8, 1\n\t"
        "or %1, %1, %8\n\t"
        "and %8, %0, %7\n\t"
        "dsrl %8, %8, 9\n\t"
        "or %1, %1, %8\n\t"
        "sd %1, -0x8(%2)\n\t"

        "ld %0, 0x10(%4)\n\t"
        "and %1, %0, %5\n\t"
        "dsll %1, %1, 11\n\t"
        "and %8, %0, %6\n\t"
        "dsll %8, %8, 1\n\t"
        "or %1, %1, %8\n\t"
        "and %8, %0, %7\n\t"
        "dsrl %8, %8, 9\n\t"
        "or %1, %1, %8\n\t"
        "sd %1, 0x0(%3)\n\t"

        "addiu %2, %3, 0x10\n\t"

        "ld %0, 0x18(%4)\n\t"
        "and %1, %0, %5\n\t"
        "dsll %1, %1, 11\n\t"
        "and %8, %0, %6\n\t"
        "dsll %8, %8, 1\n\t"
        "or %1, %1, %8\n\t"
        "and %8, %0, %7\n\t"
        "dsrl %8, %8, 9\n\t"
        "or %1, %1, %8\n\t"
        "sd %1, -0x8(%2)\n\t"

        ".set gp=default\n\t"
        ".set at\n\t"

        : "=&r" (clh1), "=&r" (clh2), "=&r" (fbaddr)
        : "2" (fbaddr), "r" (pixmem + (i * 160) + j), "r" (5), "r" (6), "r" (7), "r" (8)
        : "memory"
      );
    }

    fbaddr += (320 - 160) * 2;
  }
}